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
	
	protected void ThinkIdle(float deltaTime)
	{
		m_IdleTimer += deltaTime;
		
		PlayerBase detectedPlayer = m_Bot.GetDetection().ScanForPlayers();
		if (detectedPlayer)
		{
			m_Bot.SetTarget(detectedPlayer);
			OnPlayerDetected(detectedPlayer);
			return;
		}
		
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
	
	protected void ThinkPatrol(float deltaTime)
	{
		PlayerBase detectedPlayer = m_Bot.GetDetection().ScanForPlayers();
		if (detectedPlayer)
		{
			m_Bot.SetTarget(detectedPlayer);
			OnPlayerDetected(detectedPlayer);
			return;
		}
		
		ABBotPatrol patrol = m_Bot.GetPatrol();
		if (patrol)
		{
			patrol.UpdatePatrol(deltaTime);
		}
	}
	
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
		
		m_ReactionTimer += deltaTime;
		
		if (!m_HasReacted && m_ReactionTimer >= diff.ReactionTime)
		{
			m_HasReacted = true;
			m_ReactionTimer = 0;
			
			float dist = m_Bot.DistanceToTarget();
			
			if (dist > diff.StealthDistance)
			{
				m_Bot.SetState(ABBotState.STEALTH);
				string stealthTargetName = "Unknown";
				if (target.GetIdentity())
					stealthTargetName = target.GetIdentity().GetName();
				ABLogger.LogStealth(m_Bot.GetName(), stealthTargetName);
			}
			else
			{
				bool hasGun = false;
				PlayerBase detectEnt = m_Bot.GetEntity();
				if (detectEnt)
				{
					Weapon_Base detectWpn = Weapon_Base.Cast(detectEnt.GetItemInHands());
					if (detectWpn)
						hasGun = true;
				}
				
				if (!hasGun && dist <= ABConfig.s_Settings.MeleeEngageDistance)
				{
					m_Bot.SetState(ABBotState.COMBAT_MELEE);
					string targetName1 = "Unknown";
					if (target.GetIdentity())
						targetName1 = target.GetIdentity().GetName();
					ABLogger.LogCombatStart(m_Bot.GetName(), targetName1, "MELEE");
				}
				else
				{
					m_Bot.SetState(ABBotState.COMBAT_RANGED);
					string targetName2 = "Unknown";
					if (target.GetIdentity())
						targetName2 = target.GetIdentity().GetName();
					ABLogger.LogCombatStart(m_Bot.GetName(), targetName2, "RANGED");
				}
			}
		}
	}
	
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
		
		if (dist <= combatDist || dist <= meleeDist)
		{
			m_Bot.SetState(ABBotState.COMBAT_RANGED);
			string tn2 = "Unknown";
			if (target.GetIdentity())
				tn2 = target.GetIdentity().GetName();
			ABLogger.LogCombatStart(m_Bot.GetName(), tn2, "RANGED");
			m_StealthTimer = 0;
			return;
		}
		
		float stealthSpeed = 0.5;
		if (diff)
			stealthSpeed = diff.StealthSpeedMultiplier;
		
		m_Bot.MoveTo(target.GetPosition(), stealthSpeed);
		m_Bot.LookAt(target.GetPosition());
	}
	
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
			m_Bot.GetCombat().LowerWeapon(m_Bot.GetEntity());
			return;
		}
		
		m_IsInCombat = true;
		
		if (!m_Bot.GetCombat().IsWeaponRaised())
			m_Bot.GetCombat().RaiseWeapon(m_Bot.GetEntity());
		
		float dist = m_Bot.DistanceToTarget();
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		
		float maxDetection = 100.0;
		if (diff)
			maxDetection = diff.DetectionRadius;
		
		if (dist > maxDetection * 1.2)
		{
			if (diff && Math.RandomFloat01() < diff.Brutality)
			{
				m_Bot.SetState(ABBotState.STEALTH);
				return;
			}
			
			m_Bot.ClearTarget();
			m_Bot.SetState(ABBotState.PATROLLING);
			m_CombatTimer = 0;
			m_IsInCombat = false;
			m_Bot.GetCombat().LowerWeapon(m_Bot.GetEntity());
			return;
		}
		
		m_Bot.LookAt(target.GetPosition());
		
		float fireInterval = 1.0;
		if (diff)
			fireInterval = 1.0 / diff.FireRate;
		
		if (m_LastShotTime >= fireInterval)
		{
			m_LastShotTime = 0;
			m_Bot.GetCombat().FireAtTarget(target);
		}
		
		if (diff && Math.RandomFloat01() < diff.DodgeChance * deltaTime)
		{
			DoDodge();
		}
		
		if (dist > 15.0)
		{
			float moveSpeed = 0.8;
			if (diff)
				moveSpeed = diff.MovementSpeedMultiplier * 0.8;
			m_Bot.MoveTo(target.GetPosition(), moveSpeed);
		}
	}
	
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
		
		PlayerBase meleeEnt = m_Bot.GetEntity();
		if (meleeEnt)
		{
			Weapon_Base meleeWpn = Weapon_Base.Cast(meleeEnt.GetItemInHands());
			if (meleeWpn)
			{
				m_Bot.SetState(ABBotState.COMBAT_RANGED);
				return;
			}
		}
		
		float dist = m_Bot.DistanceToTarget();
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		
		float meleeDist = 3.0;
		if (ABConfig.s_Settings)
			meleeDist = ABConfig.s_Settings.MeleeEngageDistance;
		
		if (dist > meleeDist * 2.0)
		{
			m_Bot.SetState(ABBotState.COMBAT_RANGED);
			return;
		}
		
		m_Bot.LookAt(target.GetPosition());
		m_Bot.MoveTo(target.GetPosition(), 1.2);
		
		m_Bot.GetCombat().MeleeAttack(target);
	}
	
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
