// AloneZ BOTS - Bot Brain / AI Logic Controller
// Controla a logica de decisao do bot (FSM simplificado)

class ABBotBrain
{
	protected ABBot m_Bot;
	protected float m_ThinkTimer;
	protected float m_CombatTimer;
	protected float m_StealthTimer;
	protected float m_IdleTimer;
	protected bool m_IsInCombat;
	protected float m_LastShotTime;
	protected float m_ReactionTimer;
	protected bool m_HasReacted;
	
	void ABBotBrain(ABBot bot)
	{
		m_Bot = bot;
		m_ThinkTimer = 0;
		m_CombatTimer = 0;
		m_StealthTimer = 0;
		m_IdleTimer = 0;
		m_IsInCombat = false;
		m_LastShotTime = 0;
		m_ReactionTimer = 0;
		m_HasReacted = false;
	}
	
	void Think(float deltaTime)
	{
		if (!m_Bot || !m_Bot.IsAlive())
			return;
		
		m_ThinkTimer += deltaTime;
		
		ABBotState currentState = m_Bot.GetState();
		
		switch (currentState)
		{
			case ABBotState.IDLE:
				ThinkIdle(deltaTime);
				break;
				
			case ABBotState.PATROLLING:
				ThinkPatrol(deltaTime);
				break;
				
			case ABBotState.DETECTING:
				ThinkDetecting(deltaTime);
				break;
				
			case ABBotState.STEALTH:
				ThinkStealth(deltaTime);
				break;
				
			case ABBotState.COMBAT_RANGED:
				ThinkCombatRanged(deltaTime);
				break;
				
			case ABBotState.COMBAT_MELEE:
				ThinkCombatMelee(deltaTime);
				break;
		}
	}
	
	// --- IDLE ---
	protected void ThinkIdle(float deltaTime)
	{
		m_IdleTimer += deltaTime;
		
		// Verificar se ha player por perto
		PlayerBase detectedPlayer = m_Bot.GetDetection().ScanForPlayers();
		if (detectedPlayer)
		{
			m_Bot.SetTarget(detectedPlayer);
			OnPlayerDetected(detectedPlayer);
			return;
		}
		
		// Apos 2 segundos idle, iniciar patrulha
		if (m_IdleTimer >= 2.0)
		{
			ABBotPatrol patrol = m_Bot.GetPatrol();
			if (patrol && patrol.HasWaypoints())
			{
				m_Bot.SetState(ABBotState.PATROLLING);
				m_IdleTimer = 0;
			}
		}
	}
	
	// --- PATROLLING ---
	protected void ThinkPatrol(float deltaTime)
	{
		// Verificar deteccao de player
		PlayerBase detectedPlayer = m_Bot.GetDetection().ScanForPlayers();
		if (detectedPlayer)
		{
			m_Bot.SetTarget(detectedPlayer);
			OnPlayerDetected(detectedPlayer);
			return;
		}
		
		// Continuar patrulha
		ABBotPatrol patrol = m_Bot.GetPatrol();
		if (patrol)
		{
			patrol.UpdatePatrol(deltaTime);
		}
	}
	
	// --- DETECTING (player encontrado, preparando acao) ---
	protected void ThinkDetecting(float deltaTime)
	{
		PlayerBase target = m_Bot.GetTarget();
		if (!target || !target.IsAlive())
		{
			m_Bot.ClearTarget();
			m_Bot.SetState(ABBotState.PATROLLING);
			m_HasReacted = false;
			m_ReactionTimer = 0;
			return;
		}
		
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		if (!diff)
		{
			m_Bot.SetState(ABBotState.STEALTH);
			return;
		}
		
		// Tempo de reacao baseado na dificuldade
		m_ReactionTimer += deltaTime;
		
		if (!m_HasReacted && m_ReactionTimer >= diff.ReactionTime)
		{
			m_HasReacted = true;
			m_ReactionTimer = 0;
			
			float dist = m_Bot.DistanceToTarget();
			
			// Decidir modo: furtivo ou combate direto
			if (dist > diff.StealthDistance)
			{
				m_Bot.SetState(ABBotState.STEALTH);
				ABLogger.LogStealth(m_Bot.GetName(), target.GetIdentity() ? target.GetIdentity().GetName() : "Unknown");
			}
			else if (dist <= ABConfig.s_Settings.MeleeEngageDistance && Math.RandomFloat01() < diff.MeleeChance)
			{
				m_Bot.SetState(ABBotState.COMBAT_MELEE);
				string targetName1 = target.GetIdentity() ? target.GetIdentity().GetName() : "Unknown";
				ABLogger.LogCombatStart(m_Bot.GetName(), targetName1, "MELEE");
			}
			else
			{
				m_Bot.SetState(ABBotState.COMBAT_RANGED);
				string targetName2 = target.GetIdentity() ? target.GetIdentity().GetName() : "Unknown";
				ABLogger.LogCombatStart(m_Bot.GetName(), targetName2, "RANGED");
			}
		}
	}
	
	// --- STEALTH (aproximacao furtiva) ---
	protected void ThinkStealth(float deltaTime)
	{
		m_StealthTimer += deltaTime;
		
		PlayerBase target = m_Bot.GetTarget();
		if (!target || !target.IsAlive())
		{
			m_Bot.ClearTarget();
			m_Bot.SetState(ABBotState.PATROLLING);
			m_StealthTimer = 0;
			return;
		}
		
		float dist = m_Bot.DistanceToTarget();
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		
		// Se perdeu visao (muito longe), voltar a patrulhar
		float maxDetection = 100.0;
		if (diff)
			maxDetection = diff.DetectionRadius;
		
		if (dist > maxDetection * 1.5)
		{
			m_Bot.ClearTarget();
			m_Bot.SetState(ABBotState.PATROLLING);
			m_StealthTimer = 0;
			return;
		}
		
		float combatDist = 30.0;
		if (ABConfig.s_Settings)
			combatDist = ABConfig.s_Settings.CombatEngageDistance;
		
		float meleeDist = 3.0;
		if (ABConfig.s_Settings)
			meleeDist = ABConfig.s_Settings.MeleeEngageDistance;
		
		// Chegou perto o suficiente para combate
		if (dist <= meleeDist && diff && Math.RandomFloat01() < diff.MeleeChance)
		{
			m_Bot.SetState(ABBotState.COMBAT_MELEE);
			string tn1 = target.GetIdentity() ? target.GetIdentity().GetName() : "Unknown";
			ABLogger.LogCombatStart(m_Bot.GetName(), tn1, "MELEE");
			m_StealthTimer = 0;
			return;
		}
		
		if (dist <= combatDist)
		{
			m_Bot.SetState(ABBotState.COMBAT_RANGED);
			string tn2 = target.GetIdentity() ? target.GetIdentity().GetName() : "Unknown";
			ABLogger.LogCombatStart(m_Bot.GetName(), tn2, "RANGED");
			m_StealthTimer = 0;
			return;
		}
		
		// Mover furtivamente em direcao ao alvo
		float stealthSpeed = 0.5;
		if (diff)
			stealthSpeed = diff.StealthSpeedMultiplier;
		
		m_Bot.MoveTo(target.GetPosition(), stealthSpeed);
		m_Bot.LookAt(target.GetPosition());
	}
	
	// --- COMBAT RANGED ---
	protected void ThinkCombatRanged(float deltaTime)
	{
		m_CombatTimer += deltaTime;
		m_LastShotTime += deltaTime;
		
		PlayerBase target = m_Bot.GetTarget();
		if (!target || !target.IsAlive())
		{
			m_Bot.ClearTarget();
			m_Bot.SetState(ABBotState.PATROLLING);
			m_CombatTimer = 0;
			m_IsInCombat = false;
			return;
		}
		
		m_IsInCombat = true;
		
		float dist = m_Bot.DistanceToTarget();
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		
		// Perdeu o alvo (muito longe)
		float maxDetection = 100.0;
		if (diff)
			maxDetection = diff.DetectionRadius;
		
		if (dist > maxDetection * 1.2)
		{
			// Verificar brutalidade - chance de perseguir
			if (diff && Math.RandomFloat01() < diff.Brutality)
			{
				m_Bot.SetState(ABBotState.STEALTH);
				return;
			}
			
			m_Bot.ClearTarget();
			m_Bot.SetState(ABBotState.PATROLLING);
			m_CombatTimer = 0;
			m_IsInCombat = false;
			return;
		}
		
		float meleeDist = 3.0;
		if (ABConfig.s_Settings)
			meleeDist = ABConfig.s_Settings.MeleeEngageDistance;
		
		// Trocar para melee se estiver muito perto
		if (dist <= meleeDist && diff && Math.RandomFloat01() < diff.MeleeChance)
		{
			m_Bot.SetState(ABBotState.COMBAT_MELEE);
			return;
		}
		
		// Olhar para o alvo
		m_Bot.LookAt(target.GetPosition());
		
		// Atirar
		float fireInterval = 1.0;
		if (diff)
			fireInterval = 1.0 / diff.FireRate;
		
		if (m_LastShotTime >= fireInterval)
		{
			m_LastShotTime = 0;
			m_Bot.GetCombat().FireAtTarget(target);
		}
		
		// Dodge (esquiva)
		if (diff && Math.RandomFloat01() < diff.DodgeChance * deltaTime)
		{
			DoDodge();
		}
		
		// Movimentacao em combate - aproximar lentamente
		if (dist > 15.0)
		{
			float moveSpeed = 0.8;
			if (diff)
				moveSpeed = diff.MovementSpeedMultiplier * 0.8;
			m_Bot.MoveTo(target.GetPosition(), moveSpeed);
		}
	}
	
	// --- COMBAT MELEE ---
	protected void ThinkCombatMelee(float deltaTime)
	{
		m_CombatTimer += deltaTime;
		
		PlayerBase target = m_Bot.GetTarget();
		if (!target || !target.IsAlive())
		{
			m_Bot.ClearTarget();
			m_Bot.SetState(ABBotState.PATROLLING);
			m_CombatTimer = 0;
			m_IsInCombat = false;
			return;
		}
		
		m_IsInCombat = true;
		
		float dist = m_Bot.DistanceToTarget();
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		
		// Se o alvo fugiu, voltar para ranged
		float meleeDist = 3.0;
		if (ABConfig.s_Settings)
			meleeDist = ABConfig.s_Settings.MeleeEngageDistance;
		
		if (dist > meleeDist * 2.0)
		{
			m_Bot.SetState(ABBotState.COMBAT_RANGED);
			return;
		}
		
		// Olhar e mover para o alvo
		m_Bot.LookAt(target.GetPosition());
		m_Bot.MoveTo(target.GetPosition(), 1.2);
		
		// Atacar corpo a corpo
		m_Bot.GetCombat().MeleeAttack(target);
	}
	
	// --- Helpers ---
	
	protected void OnPlayerDetected(PlayerBase player)
	{
		string playerName = "Unknown";
		if (player.GetIdentity())
			playerName = player.GetIdentity().GetName();
		
		float dist = m_Bot.DistanceToTarget();
		
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogDetection)
		{
			ABLogger.LogDetection(m_Bot.GetName(), playerName, dist);
		}
		
		m_Bot.SetState(ABBotState.DETECTING);
		m_HasReacted = false;
		m_ReactionTimer = 0;
		
		// Alertar grupo
		ABBotGroup group = m_Bot.GetGroup();
		if (group)
			group.AlertGroup(player, m_Bot);
	}
	
	protected void DoDodge()
	{
		vector currentPos = m_Bot.GetPosition();
		float dodgeDir = Math.RandomFloat(-1.0, 1.0);
		
		vector dodgeOffset;
		if (dodgeDir > 0)
			dodgeOffset = Vector(Math.RandomFloat(2.0, 4.0), 0, Math.RandomFloat(-1.0, 1.0));
		else
			dodgeOffset = Vector(Math.RandomFloat(-4.0, -2.0), 0, Math.RandomFloat(-1.0, 1.0));
		
		m_Bot.MoveTo(currentPos + dodgeOffset, 1.5);
	}
	
	bool IsInCombat()
	{
		return m_IsInCombat;
	}
};
