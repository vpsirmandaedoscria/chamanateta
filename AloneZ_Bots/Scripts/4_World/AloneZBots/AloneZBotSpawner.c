// ============================================================================
// AloneZ Bots — Spawner de Bots
// Le configs e instancia bots nas rotas configuradas
// ============================================================================

class AloneZBotSpawner
{
    protected ref array<ref AloneZBotGroup> m_Groups;
    protected int m_TotalBotsSpawned;
    protected bool m_Initialized;
    protected float m_UpdateTimer;
    protected float m_UpdateInterval;

    // Instancia estatica para acesso global (respawn)
    static ref AloneZBotSpawner s_Instance;

    void AloneZBotSpawner()
    {
        m_Groups = new array<ref AloneZBotGroup>;
        m_TotalBotsSpawned = 0;
        m_Initialized = false;
        m_UpdateTimer = 0;
        m_UpdateInterval = 100; // Update a cada 100ms
        s_Instance = this;
    }

    void Initialize()
    {
        if (m_Initialized)
            return;

        array<ref AloneZRouteConfig> routes = AloneZBotsConfig.GetRoutes();
        if (!routes || routes.Count() == 0)
        {
            AloneZBotsLogger.LogWarning("MOD_START", "Nenhuma rota configurada. Nenhum bot sera spawnado.");
            m_Initialized = true;
            return;
        }

        int maxBots = AloneZBotsConfig.GetGlobalSettings().MaxBotsTotal;

        foreach (AloneZRouteConfig route : routes)
        {
            if (!route.Enabled)
                continue;

            // Verifica limite total de bots
            if (m_TotalBotsSpawned + route.BotCount > maxBots)
            {
                string maxBotsStr = maxBots.ToString();
                AloneZBotsLogger.LogWarning("MOD_START", "Limite maximo de bots (" + maxBotsStr + ") atingido. Rota '" + route.RouteName + "' nao sera spawnada.");
                continue;
            }

            // Verifica se a rota tem waypoints
            if (!route.Waypoints || route.Waypoints.Count() == 0)
            {
                AloneZBotsLogger.LogError("MOD_START", "Rota '" + route.RouteName + "' nao tem waypoints. Ignorando.");
                continue;
            }

            // Verifica schedule
            if (route.Schedule && route.Schedule.UseSchedule)
            {
                if (!IsWithinSchedule(route.Schedule))
                {
                    AloneZBotsLogger.LogInfo("MOD_START", "Rota '" + route.RouteName + "' fora do horario ativo. Ignorando.");
                    continue;
                }
            }

            // Spawna o grupo
            SpawnGroup(route);
        }

        m_Initialized = true;

        // Inicia timer de update
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(OnUpdate, 100, true);
    }

    protected void SpawnGroup(AloneZRouteConfig route)
    {
        AloneZBotGroup group = new AloneZBotGroup(route);

        // Posicao inicial = primeiro waypoint
        vector spawnPos = route.Waypoints[0].Position;

        // Ajusta Y ao terreno
        spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);

        // Carrega loadout
        AloneZLoadoutConfig loadout = AloneZBotsConfig.GetLoadout(route.Loadout);

        for (int i = 0; i < route.BotCount; i++)
        {
            // Seleciona aparencia do bot
            string className = "SurvivorM_Mirek"; // Default
            if (route.BotAppearance && route.BotAppearance.ClassNames && route.BotAppearance.ClassNames.Count() > 0)
            {
                if (route.BotAppearance.RandomizeAppearance)
                {
                    int randIdx = Math.RandomInt(0, route.BotAppearance.ClassNames.Count());
                    className = route.BotAppearance.ClassNames[randIdx];
                }
                else
                {
                    int idx = i % route.BotAppearance.ClassNames.Count();
                    className = route.BotAppearance.ClassNames[idx];
                }
            }

            // Calcula posicao com offset de formacao
            vector botPos = spawnPos + group.GetFormationOffset(i);
            botPos[1] = GetGame().SurfaceY(botPos[0], botPos[2]);

            // Cria survivor vanilla e wraps com controller
            Object newObj = GetGame().CreateObject(className, botPos, false, false);
            PlayerBase player = PlayerBase.Cast(newObj);

            if (!player)
            {
                string iStr = i.ToString();
                AloneZBotsLogger.LogError("BOT_SPAWN", "Falha ao criar bot #" + iStr + " para rota '" + route.RouteName + "'.");
                continue;
            }

            // Cria wrapper controller
            string botName = className + "_" + i.ToString();
            AloneZBotEntity bot = new AloneZBotEntity(player);
            bot.InitBot(botName, route, i);

            // Aplica loadout
            if (loadout)
                bot.ApplyLoadout(loadout);

            // Aplica multiplicador de vida
            if (route.BotHealth && route.BotHealth.HealthMultiplier != 1.0)
            {
                float baseHealth = bot.GetHealth("", "Health");
                bot.SetHealth("", "Health", baseHealth * route.BotHealth.HealthMultiplier);
            }

            // Adiciona ao grupo
            group.AddMember(bot);
            m_TotalBotsSpawned++;
        }

        m_Groups.Insert(group);

        int spawnedCount = route.BotCount;
        string startMsg = "Rota '" + route.RouteName + "' (" + route.RouteID + ") iniciada. " + spawnedCount.ToString() + " bots spawnados.";
        AloneZBotsLogger.LogInfo("ROUTE_START", startMsg);
    }

    // Update periodico
    void OnUpdate()
    {
        if (!m_Initialized)
            return;

        float deltaTime = m_UpdateInterval / 1000.0; // 100/1000 = 0.1 segundos

        foreach (AloneZBotGroup group : m_Groups)
        {
            if (group)
                group.Update(deltaTime);
        }
    }

    // Respawn de grupo (chamado pelo grupo quando todos morrem)
    static void RespawnGroup(AloneZBotGroup group)
    {
        if (!s_Instance || !group)
            return;

        AloneZRouteConfig route = group.GetRouteConfig();
        if (!route)
            return;

        // Limpa membros antigos
        group.Cleanup();

        // Remove grupo antigo
        int groupIdx = s_Instance.m_Groups.Find(group);
        if (groupIdx != -1)
            s_Instance.m_Groups.Remove(groupIdx);

        // Spawna novo grupo
        s_Instance.SpawnGroup(route);
    }

    // Verifica se esta dentro do horario configurado
    protected bool IsWithinSchedule(AloneZScheduleConfig schedule)
    {
        if (!schedule || !schedule.UseSchedule)
            return true;

        int year, month, day, hour, minute, second;
        GetYearMonthDay(year, month, day);
        GetHourMinuteSecond(hour, minute, second);

        // Verifica horario
        if (hour < schedule.ActiveHoursStart || hour >= schedule.ActiveHoursEnd)
            return false;

        // Verifica dia da semana (1=segunda, 7=domingo)
        if (schedule.DaysOfWeek && schedule.DaysOfWeek.Count() > 0)
        {
            // DayZ nao tem funcao nativa de dia da semana,
            // entao por padrao permite todos os dias
            // O admin pode desabilitar dias especificos
        }

        return true;
    }

    // --- Getters ---

    int GetRouteCount()
    {
        return m_Groups.Count();
    }

    int GetTotalBotCount()
    {
        return m_TotalBotsSpawned;
    }

    array<ref AloneZBotGroup> GetGroups()
    {
        return m_Groups;
    }

    // Cleanup geral
    void Cleanup()
    {
        // Para timer de update
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(OnUpdate);

        // Remove todos os grupos e bots
        foreach (AloneZBotGroup group : m_Groups)
        {
            if (group)
                group.Cleanup();
        }

        m_Groups.Clear();
        m_TotalBotsSpawned = 0;
        m_Initialized = false;

        AloneZBotsLogger.LogInfo("MOD_STOP", "Todos os bots removidos. Spawner limpo.");
    }

    // Reload de configs em runtime
    void ReloadConfigs()
    {
        AloneZBotsLogger.LogInfo("CONFIG_RELOAD", "Recarregando configuracoes...");

        int oldRouteCount = m_Groups.Count();

        // Limpa tudo
        Cleanup();

        // Recarrega configs
        AloneZBotsConfig.Load();

        // Re-inicializa
        Initialize();

        int newRouteCount = m_Groups.Count();
        int diff = newRouteCount - oldRouteCount;

        string diffStr = "";
        if (diff > 0)
        {
            string diffPosStr = diff.ToString();
            diffStr = diffPosStr + " rota(s) nova(s) adicionada(s).";
        }
        else if (diff < 0)
        {
            int absDiff = Math.AbsInt(diff);
            string diffNegStr = absDiff.ToString();
            diffStr = diffNegStr + " rota(s) removida(s).";
        }
        else
        {
            diffStr = "Nenhuma mudanca no numero de rotas.";
        }

        string newCountStr = newRouteCount.ToString();
        string reloadMsg = "Configuracoes recarregadas. " + newCountStr + " rotas ativas. " + diffStr;
        AloneZBotsLogger.LogInfo("CONFIG_RELOAD", reloadMsg);
    }
}
