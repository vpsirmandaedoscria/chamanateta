// ============================================================================
// AloneZ Bots — Classe Principal do Bot (Controller/Wrapper)
// Wraps a vanilla PlayerBase entity with bot AI behavior
// Usa HumanInputController para movimento com animacao
// Usa ProcessWeaponEvent para tiros reais com som
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

    // Estado de movimento via InputController
    protected float m_MoveSpeed;
    protected float m_MoveAngle;
    protected bool m_MoveActive;
    protected bool m_MoveTimerStarted;
    protected bool m_WeaponIsRaised;

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
        m_MoveSpeed = 0;
        m_MoveAngle = 0;
        m_MoveActive = false;
        m_MoveTimerStarted = false;
        m_WeaponIsRaised = false;

        if (player)
            s_BotPlayers.Insert(player);
    }

    void ~AloneZBotEntity()
    {
        StopMoveTimer();
        if (m_Player)
            s_BotPlayers.RemoveItem(m_Player);
    }

    // --- Timer de movimento (roda a cada frame ~33ms) ---

    protected void StartMoveTimer()
    {
        if (m_MoveTimerStarted)
            return;
        m_MoveTimerStarted = true;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnMoveFrame, 33, true);
    }

    protected void StopMoveTimer()
    {
        if (!m_MoveTimerStarted)
            return;
        m_MoveTimerStarted = false;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(OnMoveFrame);
    }

    void OnMoveFrame()
    {
        if (!m_Player || !m_Player.IsAlive())
            return;

        HumanInputController hic = m_Player.GetInputController();
        if (!hic)
            return;

        if (m_MoveActive)
        {
            hic.OverrideMovementSpeed(true, m_MoveSpeed);
            hic.OverrideMovementAngle(true, m_MoveAngle);
        }
        else
        {
            hic.OverrideMovementSpeed(true, 0);
        }

        if (m_WeaponIsRaised)
        {
            hic.OverrideRaise(true, true);
        }

        HumanCommandMove moveCmd = m_Player.GetCommand_Move();
        if (moveCmd)
        {
            moveCmd.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
        }
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

        // Inicia timer de movimento para garantir que o bot anda desde o inicio
        StartMoveTimer();

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

    // --- Movimento (usa HumanInputController para animacao correta) ---

    void MoveToPosition(vector targetPos, float deltaTime)
    {
        if (!m_Player || !m_Player.IsAlive())
            return;

        vector currentPos = m_Player.GetPosition();
        vector direction = targetPos - currentPos;
        direction[1] = 0;

        float dist = direction.Length();
        if (dist < 0.5)
        {
            StopMovement();
            return;
        }

        direction.Normalize();

        // Vira o bot na direcao do movimento
        m_Player.SetDirection(direction);

        // Calcula angulo de movimento relativo a direcao do bot
        vector playerDir = m_Player.GetDirection();
        float angle = Math.Atan2(direction[0], direction[2]) - Math.Atan2(playerDir[0], playerDir[2]);

        // Velocidade: 1.0 = andar, 2.0 = correr, 3.0 = sprint
        float speed = 1.0;
        if (m_AnimHandler)
        {
            float speedVal = m_AnimHandler.GetCurrentSpeedValue();
            if (speedVal >= 3.0)
                speed = 3.0;
            else if (speedVal >= 2.0)
                speed = 2.0;
            else
                speed = 1.0;
        }

        m_MoveSpeed = speed;
        m_MoveAngle = angle;
        m_MoveActive = true;

        if (!m_MoveTimerStarted)
            StartMoveTimer();
    }

    void StopMovement()
    {
        m_MoveActive = false;
        m_MoveSpeed = 0;
        m_MoveAngle = 0;

        if (m_Player)
        {
            HumanInputController hic = m_Player.GetInputController();
            if (hic)
            {
                hic.OverrideMovementSpeed(true, 0);
            }
        }
    }

    // --- Combate (com engatilhar, mirar e tiro real) ---

    void BotFireWeapon(Object target, float accuracy, bool isHeadshot)
    {
        if (!m_Player || !m_Player.IsAlive() || !target)
            return;

        // Passo 1: Levanta arma primeiro — se nao estava levantada, espera proximo tick
        if (!m_WeaponIsRaised)
        {
            RaiseWeapon();
            return;
        }

        // Passo 2: Verifica se tem arma na mao
        Weapon_Base weapon = Weapon_Base.Cast(m_Player.GetItemInHands());
        if (!weapon)
            return;

        int mi = weapon.GetCurrentMuzzle();

        // Passo 3: Se chamber esta vazio, tenta engatilhar/recarregar
        if (weapon.IsChamberEmpty(mi))
        {
            // Tenta ciclar o mecanismo (puxar ferrolho) para chambear proxima bala
            weapon.ProcessWeaponEvent(new WeaponEventMechanism(m_Player));

            // Tenta recarregar se magazine vazio
            TryReload();
            return;
        }

        // Passo 4: Dispara tiro real (gera som + animacao + consome municao)
        weapon.ProcessWeaponEvent(new WeaponEventTrigger(m_Player));

        // Passo 5: Apos 300ms, cicla o mecanismo para chambear proxima bala
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CycleWeaponAction, 300, false);

        // Passo 6: Verifica hit baseado na accuracy
        float hitRoll = Math.RandomFloat01();
        if (hitRoll > accuracy)
            return;

        // Acertou — aplica dano
        PlayerBase hitPlayer = PlayerBase.Cast(target);
        if (!hitPlayer || !hitPlayer.IsAlive())
            return;

        if (AloneZBotEntity.IsBotPlayer(hitPlayer))
            return;

        float damage = Math.RandomFloatInclusive(18, 35);

        if (m_RouteConfig && m_RouteConfig.BotHealth)
            damage = damage * m_RouteConfig.BotHealth.DamageDealtMultiplier;

        if (isHeadshot)
            damage = damage * 2.0;

        hitPlayer.DecreaseHealth("", "", damage);

        string dmgStr = damage.ToString();
        string hitMsg = "Bot '" + m_BotName + "' acertou jogador. Dano: " + dmgStr;
        AloneZBotsLogger.LogInfo("COMBAT_HIT", hitMsg);
    }

    // Cicla o mecanismo da arma (puxa ferrolho) para chambear proxima bala
    void CycleWeaponAction()
    {
        if (!m_Player || !m_Player.IsAlive())
            return;

        Weapon_Base weapon = Weapon_Base.Cast(m_Player.GetItemInHands());
        if (!weapon)
            return;

        int mi = weapon.GetCurrentMuzzle();
        if (weapon.IsChamberEmpty(mi))
        {
            weapon.ProcessWeaponEvent(new WeaponEventMechanism(m_Player));
        }
    }

    // Tenta recarregar a arma com magazine reserva do inventario
    void TryReload()
    {
        if (!m_Player)
            return;

        Weapon_Base weapon = Weapon_Base.Cast(m_Player.GetItemInHands());
        if (!weapon)
            return;

        WeaponManager wm = m_Player.GetWeaponManager();
        if (!wm)
            return;

        int mi = weapon.GetCurrentMuzzle();
        Magazine currentMag = Magazine.Cast(weapon.GetMagazine(mi));

        // Se magazine atual ainda tem bala, so precisa ciclar
        if (currentMag && currentMag.GetAmmoCount() > 0)
            return;

        // Procura magazine reserva no inventario
        Magazine spareMag = FindSpareMagazine(weapon, mi);
        if (spareMag && wm.CanAttachMagazine(weapon, spareMag))
        {
            wm.AttachMagazine(spareMag);
            AloneZBotsLogger.LogDebug("COMBAT_RELOAD", "Bot '" + m_BotName + "' recarregou arma.");
        }
    }

    // Procura magazine compativel no inventario do bot
    protected Magazine FindSpareMagazine(Weapon_Base weapon, int mi)
    {
        if (!m_Player || !weapon)
            return null;

        GameInventory inv = m_Player.GetInventory();
        if (!inv)
            return null;

        for (int i = 0; i < inv.GetCargo().GetItemCount(); i++)
        {
            EntityAI item = inv.GetCargo().GetItem(i);
            Magazine mag = Magazine.Cast(item);
            if (mag && mag.GetAmmoCount() > 0)
            {
                if (weapon.CanAttachMagazine(mi, mag))
                    return mag;
            }
        }

        return null;
    }

    // Prepara a arma ao spawnar (engatilha se necessario)
    void PrepareWeapon()
    {
        if (!m_Player)
            return;

        Weapon_Base weapon = Weapon_Base.Cast(m_Player.GetItemInHands());
        if (!weapon)
            return;

        int mi = weapon.GetCurrentMuzzle();
        if (weapon.IsChamberEmpty(mi))
        {
            weapon.ProcessWeaponEvent(new WeaponEventMechanism(m_Player));
        }
    }

    void RaiseWeapon()
    {
        if (m_WeaponIsRaised)
            return;
        m_WeaponIsRaised = true;
        if (!m_MoveTimerStarted)
            StartMoveTimer();
    }

    void LowerWeapon()
    {
        m_WeaponIsRaised = false;
        if (m_Player)
        {
            HumanInputController hic = m_Player.GetInputController();
            if (hic)
            {
                hic.OverrideRaise(true, false);
            }
        }
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
