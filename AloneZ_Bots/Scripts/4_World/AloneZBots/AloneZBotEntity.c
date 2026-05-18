// ============================================================================
// AloneZ Bots — Classe Principal do Bot (Controller/Wrapper)
// Wraps a vanilla PlayerBase entity with bot AI behavior
// Usa SetPosition para movimento + SetDirection para facing
// Usa ProcessDirectDamage + GetCommandModifier_Weapons para combate
// ============================================================================

class AloneZBotEntity
{
    protected PlayerBase m_Player;
    protected string m_BotName;
    protected int m_MemberIndex;
    protected bool m_IsLeader;
    protected ref AloneZBotBrain m_Brain;
    protected ref AloneZBotAnimHandler m_AnimHandler;
    protected ref AloneZRouteConfig m_RouteConfig;
    protected AloneZBotGroup m_Group;
    protected bool m_IsInitialized;
    protected bool m_BotAlive;
    protected bool m_WasAlive;
    protected int m_DebugCounter;

    // Registro global de bots para deteccao de ameacas
    static ref set<PlayerBase> s_BotPlayers = new set<PlayerBase>;

    void AloneZBotEntity(PlayerBase player)
    {
        m_Player = player;
        m_BotName = "AloneZ_Bot";
        m_MemberIndex = 0;
        m_IsLeader = false;
        m_IsInitialized = false;
        m_BotAlive = true;
        m_WasAlive = true;
        m_DebugCounter = 0;

        if (player)
            s_BotPlayers.Insert(player);
    }

    void ~AloneZBotEntity()
    {
        if (m_Player)
            s_BotPlayers.RemoveItem(m_Player);
    }

    void InitBot(string botName, AloneZRouteConfig routeConfig, int memberIndex)
    {
        m_BotName = botName;
        m_MemberIndex = memberIndex;
        m_RouteConfig = routeConfig;

        m_AnimHandler = new AloneZBotAnimHandler(this);
        m_Brain = new AloneZBotBrain(this, routeConfig, m_AnimHandler);

        m_IsInitialized = true;
        m_BotAlive = true;
        m_WasAlive = true;

        string spawnMsg = "Bot '" + m_BotName + "' spawnado na rota '" + routeConfig.RouteID + "'";
        AloneZBotsLogger.LogInfo("BOT_SPAWN", spawnMsg);
    }

    // Verifica se um PlayerBase e um bot AloneZ
    static bool IsBotPlayer(PlayerBase player)
    {
        if (!player)
            return false;
        return s_BotPlayers.Find(player) != -1;
    }

    // --- Update principal (chamado pelo grupo) ---

    void UpdateAI(float deltaTime)
    {
        if (!m_IsInitialized || !m_BotAlive || !m_Player)
            return;

        // Detecta morte sem override
        if (!m_Player.IsAlive() && m_WasAlive)
        {
            m_WasAlive = false;
            m_BotAlive = false;
            if (m_Brain)
                m_Brain.OnBotKilled(null);
            return;
        }

        // Debug log a cada 10 segundos (100 updates de 0.1s)
        m_DebugCounter++;
        if (m_DebugCounter >= 100)
        {
            m_DebugCounter = 0;
            vector dbgPos = m_Player.GetPosition();
            string state = "UNKNOWN";
            if (m_Brain)
                state = m_Brain.GetStateName(m_Brain.GetCurrentState());
            string dbgMsg = "Bot '" + m_BotName + "' estado=" + state + " pos=" + dbgPos.ToString();
            Print("[AloneZ] [DEBUG] " + dbgMsg);
        }

        if (m_Brain)
            m_Brain.Update(deltaTime);
    }

    // --- Acesso ao player entity ---

    PlayerBase GetPlayer()
    {
        return m_Player;
    }

    // --- Delegacao de metodos do entity ---

    vector GetPosition()
    {
        if (m_Player)
            return m_Player.GetPosition();
        return "0 0 0";
    }

    void SetPosition(vector pos)
    {
        if (m_Player)
            m_Player.SetPosition(pos);
    }

    void SetOrientation(vector orient)
    {
        if (m_Player)
            m_Player.SetOrientation(orient);
    }

    float GetHealth(string zone, string type)
    {
        if (m_Player)
            return m_Player.GetHealth(zone, type);
        return 0;
    }

    void SetHealth(string zone, string type, float value)
    {
        if (m_Player)
            m_Player.SetHealth(zone, type, value);
    }

    void AddHealth(string zone, string type, float value)
    {
        if (m_Player)
            m_Player.AddHealth(zone, type, value);
    }

    bool IsAlive()
    {
        if (m_Player)
            return m_Player.IsAlive() && m_BotAlive;
        return false;
    }

    HumanInventory GetHumanInventory()
    {
        if (m_Player)
            return m_Player.GetHumanInventory();
        return null;
    }

    GameInventory GetInventory()
    {
        if (m_Player)
            return m_Player.GetInventory();
        return null;
    }

    // --- Movimento (usa SetPosition + SetDirection como referencia) ---

    void MoveToPosition(vector targetPos, float deltaTime)
    {
        if (!m_Player || !m_Player.IsAlive())
            return;

        vector currentPos = m_Player.GetPosition();
        vector direction = targetPos - currentPos;
        direction[1] = 0;

        float dist = direction.Length();
        if (dist < 0.5)
            return;

        direction.Normalize();

        // Vira o bot na direcao do movimento
        m_Player.SetDirection(direction);

        // Velocidades reais em m/s
        float speedMPS = 1.8;
        if (m_AnimHandler)
        {
            float speedVal = m_AnimHandler.GetCurrentSpeedValue();
            if (speedVal >= 3.0)
                speedMPS = 6.5;
            else if (speedVal >= 2.0)
                speedMPS = 4.0;
            else if (speedVal >= 1.0)
                speedMPS = 1.8;
            else
                speedMPS = 0.0;
        }

        float moveDist = speedMPS * deltaTime;
        if (moveDist > dist)
            moveDist = dist;

        vector newPos = currentPos + (direction * moveDist);
        float surfY = GetGame().SurfaceY(newPos[0], newPos[2]);
        newPos[1] = surfY;
        m_Player.SetPosition(newPos);
    }

    // --- Combate (usa ProcessDirectDamage + WeaponActions.FIRE) ---

    void BotFireWeapon(Object target, float accuracy, bool isHeadshot)
    {
        if (!m_Player || !m_Player.IsAlive() || !target)
            return;

        // Animacao de tiro via HumanCommandWeapons
        PlayFireAnimation();

        // Verifica hit baseado na accuracy (probabilidade direta)
        float hitRoll = Math.RandomFloat01();
        if (hitRoll > accuracy)
            return;

        // Acertou — aplica dano via ProcessDirectDamage
        PlayerBase hitPlayer = PlayerBase.Cast(target);
        if (!hitPlayer || !hitPlayer.IsAlive())
            return;

        if (AloneZBotEntity.IsBotPlayer(hitPlayer))
            return;

        // Dano base aleatorio
        float damage = Math.RandomFloatInclusive(18, 35);

        // Multiplicador de dano da rota
        if (m_RouteConfig && m_RouteConfig.BotHealth)
            damage = damage * m_RouteConfig.BotHealth.DamageDealtMultiplier;

        // Headshot faz dano dobrado
        if (isHeadshot)
            damage = damage * 2.0;

        // Aplica dano ao jogador
        hitPlayer.DecreaseHealth("", "", damage);

        string dmgStr = damage.ToString();
        string hitMsg = "Bot '" + m_BotName + "' acertou jogador. Dano: " + dmgStr;
        AloneZBotsLogger.LogInfo("COMBAT_HIT", hitMsg);
    }

    void PlayFireAnimation()
    {
        if (!m_Player)
            return;

        HumanCommandWeapons hcw = m_Player.GetCommandModifier_Weapons();
        if (hcw)
        {
            hcw.StartAction(WeaponActions.FIRE, 0);
        }
    }

    void RaiseWeapon()
    {
        // Handled by PlayFireAnimation
    }

    void LowerWeapon()
    {
        // Noop
    }

    // --- Look At (usa SetDirection) ---

    void LookAtPosition(vector pos)
    {
        if (!m_Player || !m_Player.IsAlive())
            return;

        vector dir = vector.Direction(m_Player.GetPosition(), pos);
        dir[1] = 0;
        dir.Normalize();
        m_Player.SetDirection(dir);
    }

    void LookAtDirection(vector dir)
    {
        if (!m_Player)
            return;
        dir[1] = 0;
        dir.Normalize();
        m_Player.SetDirection(dir);
    }

    // --- Equipamento (Loadout) ---

    void ApplyLoadout(AloneZLoadoutConfig loadout)
    {
        if (!loadout || !m_Player)
            return;

        ApplyClothing(loadout.Clothing);
        ApplyWeapons(loadout.Weapons);
        ApplyInventory(loadout.Inventory);
    }

    protected void ApplyClothing(AloneZClothingConfig clothing)
    {
        if (!clothing || !m_Player)
            return;

        if (clothing.Head != "")
            AttachItem(clothing.Head);
        if (clothing.Mask != "")
            AttachItem(clothing.Mask);
        if (clothing.Top != "")
            AttachItem(clothing.Top);
        if (clothing.Vest != "")
            AttachItem(clothing.Vest);
        if (clothing.Gloves != "")
            AttachItem(clothing.Gloves);
        if (clothing.Pants != "")
            AttachItem(clothing.Pants);
        if (clothing.Shoes != "")
            AttachItem(clothing.Shoes);
        if (clothing.Back != "")
            AttachItem(clothing.Back);
        if (clothing.Belt != "")
            AttachItem(clothing.Belt);
    }

    protected void ApplyWeapons(AloneZWeaponsConfig weapons)
    {
        if (!weapons || !m_Player)
            return;

        if (weapons.Primary && weapons.Primary.ClassName != "")
        {
            EntityAI primaryWeapon = SpawnWeapon(weapons.Primary);
            if (primaryWeapon)
            {
                m_Player.GetHumanInventory().CreateInHands(weapons.Primary.ClassName);
            }
        }

        if (weapons.Secondary && weapons.Secondary.ClassName != "")
        {
            SpawnWeapon(weapons.Secondary);
        }

        if (weapons.Melee != "")
        {
            m_Player.GetInventory().CreateInInventory(weapons.Melee);
        }
    }

    protected EntityAI SpawnWeapon(AloneZWeaponSlotConfig weaponConfig)
    {
        if (!weaponConfig || weaponConfig.ClassName == "" || !m_Player)
            return null;

        EntityAI weaponEntity = m_Player.GetInventory().CreateInInventory(weaponConfig.ClassName);
        if (!weaponEntity)
            return null;

        Weapon_Base weapon = Weapon_Base.Cast(weaponEntity);

        if (weaponConfig.Attachments)
        {
            foreach (string attachment : weaponConfig.Attachments)
            {
                if (attachment != "")
                    weaponEntity.GetInventory().CreateAttachment(attachment);
            }
        }

        if (weapon && weaponConfig.Magazine != "")
        {
            weapon.GetInventory().CreateAttachment(weaponConfig.Magazine);

            for (int i = 0; i < weaponConfig.MagazineCount - 1; i++)
            {
                m_Player.GetInventory().CreateInInventory(weaponConfig.Magazine);
            }
        }

        return weaponEntity;
    }

    protected void ApplyInventory(array<ref AloneZInventoryItemConfig> inventory)
    {
        if (!inventory || !m_Player)
            return;

        foreach (AloneZInventoryItemConfig item : inventory)
        {
            if (item && item.ClassName != "")
            {
                for (int i = 0; i < item.Quantity; i++)
                {
                    m_Player.GetInventory().CreateInInventory(item.ClassName);
                }
            }
        }
    }

    protected void AttachItem(string className)
    {
        if (className == "" || !m_Player)
            return;

        m_Player.GetInventory().CreateInInventory(className);
    }

    // --- Stance helpers ---

    string GetCurrentStanceStr()
    {
        if (m_AnimHandler)
            return m_AnimHandler.GetCurrentStance();
        return "ERECT";
    }

    float GetCurrentSpeedValue()
    {
        if (m_AnimHandler)
            return m_AnimHandler.GetCurrentSpeedValue();
        return 0;
    }

    // --- Getters/Setters ---

    string GetBotName()
    {
        return m_BotName;
    }

    void SetBotName(string name)
    {
        m_BotName = name;
    }

    AloneZBotGroup GetGroup()
    {
        return m_Group;
    }

    void SetGroup(AloneZBotGroup group)
    {
        m_Group = group;
    }

    bool IsLeader()
    {
        return m_IsLeader;
    }

    void SetIsLeader(bool isLeader)
    {
        m_IsLeader = isLeader;
    }

    int GetMemberIndex()
    {
        return m_MemberIndex;
    }

    void SetMemberIndex(int index)
    {
        m_MemberIndex = index;
    }

    AloneZBotBrain GetBrain()
    {
        return m_Brain;
    }

    AloneZBotAnimHandler GetAnimHandler()
    {
        return m_AnimHandler;
    }

    AloneZRouteConfig GetRouteConfig()
    {
        return m_RouteConfig;
    }

    bool IsBotAlive()
    {
        return m_BotAlive && m_Player && m_Player.IsAlive();
    }

    void OnGroupCombatAlert(Object target)
    {
        if (m_Brain)
            m_Brain.OnGroupCombatAlert(target);
    }

    // Deleta o player entity do mundo
    void DeletePlayer()
    {
        if (m_Player)
        {
            s_BotPlayers.RemoveItem(m_Player);
            GetGame().ObjectDelete(m_Player);
            m_Player = null;
        }
    }
}
