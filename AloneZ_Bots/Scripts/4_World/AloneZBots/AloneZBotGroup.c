// ============================================================================
// AloneZ Bots — Gerenciamento de Grupo de Bots por Rota
// ============================================================================

class AloneZBotGroup
{
    protected string m_GroupName;
    protected string m_RouteID;
    protected ref AloneZRouteConfig m_RouteConfig;
    protected ref array<ref AloneZBotEntity> m_Members;
    protected AloneZBotEntity m_Leader;
    protected bool m_InCombat;
    protected float m_RespawnTimer;
    protected int m_CycleCount;

    void AloneZBotGroup(AloneZRouteConfig routeConfig)
    {
        m_RouteConfig = routeConfig;
        m_GroupName = routeConfig.RouteName;
        m_RouteID = routeConfig.RouteID;
        m_Members = new array<ref AloneZBotEntity>;
        m_InCombat = false;
        m_RespawnTimer = 0;
        m_CycleCount = 0;
    }

    void AddMember(AloneZBotEntity bot)
    {
        if (bot && m_Members.Find(bot) == -1)
        {
            m_Members.Insert(bot);
            bot.SetGroup(this);

            if (!m_Leader)
            {
                m_Leader = bot;
                bot.SetIsLeader(true);
            }
        }
    }

    void RemoveMember(AloneZBotEntity bot)
    {
        int idx = m_Members.Find(bot);
        if (idx != -1)
        {
            m_Members.Remove(idx);

            if (bot == m_Leader)
            {
                m_Leader = null;
                if (m_Members.Count() > 0)
                {
                    m_Leader = m_Members[0];
                    m_Leader.SetIsLeader(true);
                }
            }
        }
    }

    void Update(float deltaTime)
    {
        // Atualiza cada bot do grupo
        for (int i = m_Members.Count() - 1; i >= 0; i--)
        {
            AloneZBotEntity bot = m_Members[i];
            if (bot && bot.IsAlive())
            {
                bot.UpdateAI(deltaTime);
            }
        }

        // Verifica respawn se necessario
        if (m_RouteConfig.RespawnOnComplete && GetAliveCount() == 0)
        {
            m_RespawnTimer += deltaTime;
            float delay = AloneZBotsConfig.GetGlobalSettings().BotRespawnDelaySeconds;

            if (m_RespawnTimer >= delay)
            {
                m_RespawnTimer = 0;
                RequestRespawn();
            }
        }
    }

    void NotifyCombat(Object target)
    {
        if (m_InCombat)
            return;

        m_InCombat = true;

        AloneZBotsLogger.LogWarning("ROUTE_INTERCEPTED", "Rota '" + m_GroupName + "' interceptada! Grupo entrando em combate.");

        // Notifica todos os membros vivos
        foreach (AloneZBotEntity bot : m_Members)
        {
            if (bot && bot.IsAlive())
            {
                bot.OnGroupCombatAlert(target);
            }
        }
    }

    void NotifyCombatEnd()
    {
        m_InCombat = false;

        int remainingBots = GetAliveCount();
        string combEndMsg = "Grupo '" + m_GroupName + "' desengajou. Bots restantes: " + remainingBots.ToString() + ". Retornando a rota.";
        AloneZBotsLogger.LogInfo("COMBAT_END", combEndMsg);
    }

    void OnMemberKilled(AloneZBotEntity bot, Object killer)
    {
        string killerInfo = "desconhecido";
        string weaponInfo = "";
        float killDist = 0;

        if (killer)
        {
            PlayerBase killerPlayer = PlayerBase.Cast(killer);
            if (killerPlayer && killerPlayer.GetIdentity())
            {
                string killerName = killerPlayer.GetIdentity().GetName();
                string killerId = killerPlayer.GetIdentity().GetPlainId();
                killerInfo = "jogador '" + killerName + "' (SteamID: " + killerId + ")";

                killDist = vector.Distance(bot.GetPosition(), killerPlayer.GetPosition());

                // Tenta pegar arma do killer
                EntityAI weaponInHands = killerPlayer.GetHumanInventory().GetEntityInHands();
                if (weaponInHands)
                {
                    weaponInfo = " Arma: " + weaponInHands.GetType() + ".";
                }
            }
        }

        string distInfo = "";
        if (killDist > 0)
        {
            distInfo = " Distancia: " + killDist.ToString() + "m.";
        }

        string deathMsg = "Bot '" + bot.GetBotName() + "' (" + m_RouteID + ") morto por " + killerInfo + "." + weaponInfo + distInfo;
        AloneZBotsLogger.LogWarning("BOT_DEATH", deathMsg);

        RemoveMember(bot);

        // Verifica se todos morreram
        if (GetAliveCount() == 0)
        {
            NotifyCombatEnd();
        }
    }

    void RequestRespawn()
    {
        int respawnCount = m_RouteConfig.BotCount;
        string respawnMsg = "Rota '" + m_GroupName + "' (" + m_RouteID + ") reiniciando (respawn). Spawnando " + respawnCount.ToString() + " bots.";
        AloneZBotsLogger.LogInfo("ROUTE_START", respawnMsg);

        // O spawner vai lidar com o respawn real
        AloneZBotSpawner.RespawnGroup(this);
    }

    // --- Formacao ---

    vector GetFormationOffset(int memberIndex)
    {
        vector offset = "0 0 0";
        float spacing = m_RouteConfig.FormationSpacing;

        switch (m_RouteConfig.Formation)
        {
            case "COLUMN":
                offset[2] = -spacing * memberIndex;
                break;

            case "LINE":
                offset[0] = spacing * (memberIndex - (m_RouteConfig.BotCount / 2));
                break;

            case "WEDGE":
                if (memberIndex > 0)
                {
                    int side = 1;
                    if ((memberIndex - 1) % 2 != 0) side = -1;
                    int row = ((memberIndex - 1) / 2) + 1;
                    offset[0] = side * spacing * row;
                    offset[2] = -spacing * row;
                }
                break;

            case "STAGGERED":
                int staggerSide = 1;
                if (memberIndex % 2 != 0) staggerSide = -1;
                offset[0] = staggerSide * (spacing * 0.5);
                offset[2] = -spacing * memberIndex;
                break;

            case "RANDOM":
                offset[0] = Math.RandomFloatInclusive(-spacing, spacing);
                offset[2] = Math.RandomFloatInclusive(-spacing, spacing);
                break;
        }

        return offset;
    }

    // --- Getters ---

    int GetAliveCount()
    {
        int count = 0;
        foreach (AloneZBotEntity bot : m_Members)
        {
            if (bot && bot.IsAlive())
                count++;
        }
        return count;
    }

    int GetMemberCount()
    {
        return m_Members.Count();
    }

    string GetGroupName()
    {
        return m_GroupName;
    }

    string GetRouteID()
    {
        return m_RouteID;
    }

    AloneZRouteConfig GetRouteConfig()
    {
        return m_RouteConfig;
    }

    AloneZBotEntity GetLeader()
    {
        return m_Leader;
    }

    bool IsInCombat()
    {
        return m_InCombat;
    }

    int GetCycleCount()
    {
        return m_CycleCount;
    }

    void IncrementCycle()
    {
        m_CycleCount++;
    }

    array<ref AloneZBotEntity> GetMembers()
    {
        return m_Members;
    }

    void Cleanup()
    {
        foreach (AloneZBotEntity bot : m_Members)
        {
            if (bot)
            {
                bot.DeletePlayer();
            }
        }
        m_Members.Clear();
        m_Leader = null;
    }
}
