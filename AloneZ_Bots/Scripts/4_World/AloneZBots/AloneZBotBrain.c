// ============================================================================
// AloneZ Bots — FSM (Finite State Machine) do Bot
// Gerencia estados: PATROL, IDLE, COMBAT, SEARCH, FLEE, DEAD
// ============================================================================

enum AloneZBotState
{
    PATROL,
    IDLE,
    COMBAT,
    SEARCH,
    FLEE,
    DEAD
}

class AloneZBotBrain
{
    protected AloneZBotEntity m_Bot;
    protected AloneZBotState m_CurrentState;
    protected AloneZBotState m_PreviousState;
    protected ref AloneZBotWaypointManager m_WaypointMgr;
    protected ref AloneZBotCombatHandler m_CombatHandler;
    protected AloneZBotAnimHandler m_AnimHandler;
    protected ref AloneZRouteConfig m_RouteConfig;

    protected Object m_CurrentTarget;
    protected float m_StateTimer;
    protected float m_SearchTimer;
    protected float m_ThreatCheckTimer;
    protected float m_ThreatCheckInterval;
    protected bool m_IsGroupLeader;
    protected bool m_WaitingAtWaypoint;
    protected float m_StartupTimer;
    protected bool m_StartupComplete;

    void AloneZBotBrain(AloneZBotEntity bot, AloneZRouteConfig routeConfig, AloneZBotAnimHandler animHandler)
    {
        m_Bot = bot;
        m_RouteConfig = routeConfig;
        m_AnimHandler = animHandler;
        m_CurrentState = AloneZBotState.PATROL;
        m_PreviousState = AloneZBotState.PATROL;
        m_WaypointMgr = new AloneZBotWaypointManager(routeConfig);
        m_CombatHandler = new AloneZBotCombatHandler(bot, routeConfig);
        m_StateTimer = 0;
        m_SearchTimer = 0;
        m_ThreatCheckTimer = 0;
        m_ThreatCheckInterval = 0.5; // Checa ameacas a cada 0.5s
        m_IsGroupLeader = false;
        m_WaitingAtWaypoint = false;
        m_StartupTimer = 0;
        m_StartupComplete = false;
    }

    void Update(float deltaTime)
    {
        if (m_CurrentState == AloneZBotState.DEAD)
            return;

        switch (m_CurrentState)
        {
            case AloneZBotState.PATROL:
                UpdatePatrol(deltaTime);
                break;
            case AloneZBotState.IDLE:
                UpdateIdle(deltaTime);
                break;
            case AloneZBotState.COMBAT:
                UpdateCombat(deltaTime);
                break;
            case AloneZBotState.SEARCH:
                UpdateSearch(deltaTime);
                break;
            case AloneZBotState.FLEE:
                UpdateFlee(deltaTime);
                break;
        }

        // Periodo de graca no startup (15s sem detectar ameacas para patrulhar)
        if (!m_StartupComplete)
        {
            m_StartupTimer += deltaTime;
            if (m_StartupTimer >= 15.0)
            {
                m_StartupComplete = true;
                Print("[AloneZ] Bot '" + m_Bot.GetBotName() + "' startup completo. Deteccao de ameacas ativada.");
            }
            return;
        }

        // Checa por ameacas periodicamente (exceto se morto, em combate ou fugindo)
        if (m_CurrentState != AloneZBotState.DEAD && m_CurrentState != AloneZBotState.COMBAT && m_CurrentState != AloneZBotState.FLEE)
        {
            m_ThreatCheckTimer += deltaTime;
            if (m_ThreatCheckTimer >= m_ThreatCheckInterval)
            {
                m_ThreatCheckTimer = 0;
                CheckForThreats();
            }
        }
    }

    protected void ChangeState(AloneZBotState newState)
    {
        if (m_CurrentState == newState)
            return;

        m_PreviousState = m_CurrentState;
        m_CurrentState = newState;
        m_StateTimer = 0;

        // Log de mudanca de estado
        string prevName = GetStateName(m_PreviousState);
        string newName = GetStateName(newState);
        string stateMsg = "Bot '" + m_Bot.GetBotName() + "' mudou de " + prevName + " para " + newName;
        AloneZBotsLogger.LogDebug("STATE_CHANGE", stateMsg);

        // Callback na transicao
        OnStateEnter(newState);
    }

    protected void OnStateEnter(AloneZBotState state)
    {
        switch (state)
        {
            case AloneZBotState.PATROL:
                if (m_AnimHandler)
                    m_AnimHandler.SetCombat(false);
                m_WaitingAtWaypoint = false;
                break;

            case AloneZBotState.IDLE:
                m_WaitingAtWaypoint = true;
                m_Bot.StopMovement();
                break;

            case AloneZBotState.COMBAT:
                if (m_AnimHandler)
                    m_AnimHandler.SetCombat(true);
                break;

            case AloneZBotState.SEARCH:
                m_SearchTimer = 0;
                if (m_CombatHandler)
                    m_CombatHandler.Reset();
                if (m_AnimHandler)
                    m_AnimHandler.SetCombat(false);
                break;

            case AloneZBotState.FLEE:
                if (m_AnimHandler)
                    m_AnimHandler.SetCombat(false);
                break;

            case AloneZBotState.DEAD:
                break;
        }
    }

    // --- Update por estado ---

    protected void UpdatePatrol(float deltaTime)
    {
        if (!m_WaypointMgr)
            return;

        vector targetWP = m_WaypointMgr.GetCurrentWaypoint();

        // Aplica offset de formacao se em grupo
        if (m_Bot.GetGroup() && !m_Bot.IsLeader())
        {
            int memberIdx = m_Bot.GetMemberIndex();
            vector formationOffset = m_Bot.GetGroup().GetFormationOffset(memberIdx);
            targetWP = targetWP + formationOffset;
        }

        float distToWP = vector.Distance(m_Bot.GetPosition(), targetWP);

        // Chegou no waypoint
        if (distToWP < 3.0)
        {
            string wpName = m_WaypointMgr.GetCurrentWaypointName();
            int wpIndex = m_WaypointMgr.GetCurrentIndex();

            float wpWait = m_WaypointMgr.GetCurrentWaitTime();
            string wpMsg = "Grupo '" + m_RouteConfig.RouteName + "' alcancou waypoint #" + wpIndex.ToString() + " '" + wpName + "'. Aguardando " + wpWait.ToString() + "s.";
            AloneZBotsLogger.LogDebug("WAYPOINT", wpMsg);

            // Verifica se completou a rota
            if (m_WaypointMgr.IsRouteComplete())
            {
                if (m_Bot.GetGroup())
                    m_Bot.GetGroup().IncrementCycle();

                int cycleNum = m_WaypointMgr.GetCycleCount();
                string endMsg = "Rota '" + m_RouteConfig.RouteName + "' concluida. Ciclo #" + cycleNum.ToString() + " completo.";
                AloneZBotsLogger.LogInfo("ROUTE_END", endMsg);
            }

            m_WaypointMgr.AdvanceToNext();
            ChangeState(AloneZBotState.IDLE);
            return;
        }

        // Move em direcao ao waypoint
        string speed = m_WaypointMgr.GetCurrentSpeed();
        string stance = m_WaypointMgr.GetCurrentStance();

        if (m_AnimHandler)
            m_AnimHandler.SetMovement(speed, stance);

        m_Bot.MoveToPosition(targetWP, deltaTime);
    }

    protected void UpdateIdle(float deltaTime)
    {
        m_StateTimer += deltaTime;

        if (m_AnimHandler)
            m_AnimHandler.PlayIdle();

        // Executa acao do waypoint
        string action = m_WaypointMgr.GetCurrentAction();
        if (action == "SCAN")
        {
            // Olha ao redor
            vector lookDir = m_WaypointMgr.GetCurrentLookDirection();
            m_Bot.LookAtDirection(lookDir);
        }

        float waitTime = m_WaypointMgr.GetCurrentWaitTime();
        if (m_StateTimer >= waitTime)
        {
            ChangeState(AloneZBotState.PATROL);
        }
    }

    protected void UpdateCombat(float deltaTime)
    {
        // Verifica se alvo ainda e valido
        if (!m_CurrentTarget)
        {
            OnCombatEnd();
            return;
        }

        // Verifica se alvo esta vivo
        EntityAI targetEntity = EntityAI.Cast(m_CurrentTarget);
        if (targetEntity && !targetEntity.IsAlive())
        {
            OnCombatEnd();
            return;
        }

        // Verifica se alvo saiu do alcance de desengajamento
        if (m_CombatHandler && m_CombatHandler.IsTargetOutOfRange(m_CurrentTarget))
        {
            AloneZBotsLogger.LogInfo("COMBAT_END", "Grupo '" + m_RouteConfig.RouteName + "' perdeu alvo (fora de alcance).");
            OnCombatEnd();
            return;
        }

        // Checa se deve fugir (vida baixa)
        if (m_RouteConfig.BotBehavior.FleeOnLowHealth)
        {
            float health = m_Bot.GetHealth("", "Health");
            if (health < m_RouteConfig.BotBehavior.FleeHealthThreshold)
            {
                string fleeMsg = "Bot '" + m_Bot.GetBotName() + "' fugindo (vida baixa: " + health.ToString() + ")."; 
                AloneZBotsLogger.LogWarning("COMBAT_END", fleeMsg);
                ChangeState(AloneZBotState.FLEE);
                return;
            }
        }

        // Movimento de combate — aproxima do alvo ate 10m no maximo
        float distToTarget = vector.Distance(m_Bot.GetPosition(), m_CurrentTarget.GetPosition());

        if (distToTarget > 50.0)
        {
            // Longe: corre em direcao ao alvo
            m_Bot.MoveToPosition(m_CurrentTarget.GetPosition(), deltaTime);
            if (m_AnimHandler)
                m_AnimHandler.SetMovement("JOG", "ERECT");
        }
        else if (distToTarget > 10.0)
        {
            // Medio: anda em direcao ao alvo
            m_Bot.MoveToPosition(m_CurrentTarget.GetPosition(), deltaTime);
            if (m_AnimHandler)
                m_AnimHandler.SetMovement("WALK", "ERECT");
        }
        else
        {
            // Dentro de 10m: para de se mover, fica parado mirando
            m_Bot.StopMovement();
        }

        // Olha para o alvo e engaja (mira e atira)
        m_Bot.LookAtPosition(m_CurrentTarget.GetPosition());
        if (m_CombatHandler)
            m_CombatHandler.EngageTarget(m_CurrentTarget, deltaTime);
    }

    protected void OnCombatEnd()
    {
        m_CurrentTarget = null;

        if (m_Bot.GetGroup())
            m_Bot.GetGroup().NotifyCombatEnd();

        if (m_RouteConfig.BotBehavior.SearchAfterCombat)
        {
            ChangeState(AloneZBotState.SEARCH);
        }
        else if (m_RouteConfig.BotBehavior.ReturnToRouteAfterCombat)
        {
            ChangeState(AloneZBotState.PATROL);
        }
    }

    protected void UpdateSearch(float deltaTime)
    {
        m_SearchTimer += deltaTime;

        if (m_AnimHandler)
            m_AnimHandler.SetMovement("WALK", "CROUCH");

        // Movimenta em padrao de busca (pequeno circulo ao redor da ultima posicao)
        float searchAngle = m_SearchTimer * 0.5; // Rotacao lenta
        vector searchOffset = Vector(Math.Cos(searchAngle) * 10, 0, Math.Sin(searchAngle) * 10);
        vector searchPos = m_Bot.GetPosition() + searchOffset;
        m_Bot.MoveToPosition(searchPos, deltaTime);

        // Termina busca apos tempo configurado
        if (m_SearchTimer >= m_RouteConfig.BotBehavior.SearchDurationSeconds)
        {
            m_SearchTimer = 0;
            AloneZBotsLogger.LogDebug("SEARCH", "Bot '" + m_Bot.GetBotName() + "' terminou busca. Retornando a rota.");
            ChangeState(AloneZBotState.PATROL);
        }

        // Continua checando ameacas durante busca
        CheckForThreats();
    }

    protected void UpdateFlee(float deltaTime)
    {
        m_StateTimer += deltaTime;

        if (m_CurrentTarget)
        {
            vector fleeDir = m_Bot.GetPosition() - m_CurrentTarget.GetPosition();
            fleeDir[1] = 0; // Ignora eixo Y
            fleeDir.Normalize();
            vector fleePos = m_Bot.GetPosition() + (fleeDir * 50);
            m_Bot.MoveToPosition(fleePos, deltaTime);

            if (m_AnimHandler)
                m_AnimHandler.SetMovement("SPRINT", "ERECT");
        }

        // Para de fugir apos distancia segura ou tempo
        if (m_StateTimer > 15.0)
        {
            ChangeState(AloneZBotState.PATROL);
        }
        else if (m_CurrentTarget)
        {
            float dist = vector.Distance(m_Bot.GetPosition(), m_CurrentTarget.GetPosition());
            if (dist > AloneZBotsConfig.GetGlobalSettings().BotDisengageRange * 1.5)
            {
                ChangeState(AloneZBotState.PATROL);
            }
        }
    }

    // --- Deteccao de ameacas ---

    protected void CheckForThreats()
    {
        if (!m_RouteConfig.BotBehavior.EngageOnSight)
            return;

        float detectRange = AloneZBotsConfig.GetGlobalSettings().BotDetectionRange;

        // Busca jogadores no raio de deteccao
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        Object closestThreat = null;
        float closestDist = detectRange + 1;

        foreach (Man player : players)
        {
            if (!player || !player.IsAlive())
                continue;

            // Ignora se e outro bot AloneZ
            PlayerBase pb = PlayerBase.Cast(player);
            if (pb && AloneZBotEntity.IsBotPlayer(pb))
                continue;

            // Ignora admin invisivel
            if (pb && pb.IsInvisible())
                continue;

            float dist = vector.Distance(m_Bot.GetPosition(), player.GetPosition());
            if (dist <= detectRange && dist < closestDist)
            {
                // Verificacao de linha de visao (simplificada)
                vector from = m_Bot.GetPosition();
                from[1] = from[1] + 1.5;
                vector to = player.GetPosition();
                to[1] = to[1] + 1.2;

                // Raycast basico para LOS
                vector hitPos;
                vector hitNormal;
                int contactComponent;
                Object hitObject;

                if (DayZPhysics.RaycastRV(from, to, hitPos, hitNormal, contactComponent, null, null, player, false, false, ObjIntersectFire))
                {
                    closestThreat = player;
                    closestDist = dist;
                }
                else
                {
                    // Sem obstrucao, alvo visivel
                    closestThreat = player;
                    closestDist = dist;
                }
            }
        }

        if (closestThreat)
        {
            m_CurrentTarget = closestThreat;

            string interceptMsg = "Rota '" + m_RouteConfig.RouteName + "' interceptada! Bot '" + m_Bot.GetBotName() + "' detectou ameaca. Grupo entrando em combate.";
            AloneZBotsLogger.LogWarning("ROUTE_INTERCEPTED", interceptMsg);

            int aliveCount = GetGroupAliveCount();
            string combatMsg = "Grupo '" + m_RouteConfig.RouteName + "' engajou alvo a " + closestDist.ToString() + "m. Bots ativos: " + aliveCount.ToString() + ".";
            AloneZBotsLogger.LogInfo("COMBAT_START", combatMsg);

            // Notifica grupo
            if (m_Bot.GetGroup())
                m_Bot.GetGroup().NotifyCombat(closestThreat);

            ChangeState(AloneZBotState.COMBAT);
        }
    }

    // --- Callbacks externos ---

    void OnGroupCombatAlert(Object target)
    {
        if (m_CurrentState == AloneZBotState.COMBAT || m_CurrentState == AloneZBotState.DEAD)
            return;

        m_CurrentTarget = target;
        ChangeState(AloneZBotState.COMBAT);
    }

    void OnBotKilled(Object killer)
    {
        ChangeState(AloneZBotState.DEAD);

        if (m_AnimHandler)
            m_AnimHandler.PlayDeath();

        // Log de morte tratado pelo grupo
        if (m_Bot.GetGroup())
            m_Bot.GetGroup().OnMemberKilled(m_Bot, killer);
    }

    // --- Helpers ---

    int GetGroupAliveCount()
    {
        if (m_Bot.GetGroup())
            return m_Bot.GetGroup().GetAliveCount();
        return 1;
    }

    string GetStateName(AloneZBotState state)
    {
        switch (state)
        {
            case AloneZBotState.PATROL: return "PATROL";
            case AloneZBotState.IDLE: return "IDLE";
            case AloneZBotState.COMBAT: return "COMBAT";
            case AloneZBotState.SEARCH: return "SEARCH";
            case AloneZBotState.FLEE: return "FLEE";
            case AloneZBotState.DEAD: return "DEAD";
        }
        return "UNKNOWN";
    }

    AloneZBotState GetCurrentState()
    {
        return m_CurrentState;
    }

    AloneZBotWaypointManager GetWaypointManager()
    {
        return m_WaypointMgr;
    }

    AloneZBotCombatHandler GetCombatHandler()
    {
        return m_CombatHandler;
    }

    Object GetCurrentTarget()
    {
        return m_CurrentTarget;
    }

    void SetCurrentTarget(Object target)
    {
        m_CurrentTarget = target;
    }
}
