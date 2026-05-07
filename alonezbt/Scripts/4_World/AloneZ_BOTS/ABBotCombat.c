// AloneZ BOTS - Combat System
// Combate ranged (disparo) e melee (faca)

class ABBotCombat
{
	protected ABBot m_Bot;
	protected float m_MeleeTimer;
	protected float m_MeleeCooldown;
	protected int m_ShotsFired;
	protected int m_ShotsHit;
	
	void ABBotCombat(ABBot bot)
	{
		m_Bot = bot;
		m_MeleeTimer = 0;
		m_MeleeCooldown = 1.5;
		m_ShotsFired = 0;
		m_ShotsHit = 0;
	}
	
	// --- RANGED COMBAT ---
	
	void FireAtTarget(PlayerBase target)
	{
		if (!m_Bot || !m_Bot.IsAlive() || !target || !target.IsAlive())
			return;
		
		PlayerBase botEntity = m_Bot.GetEntity();
		if (!botEntity)
			return;
		
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		if (!diff)
			return;
		
		m_ShotsFired++;
		
		// Calcular precisao
		float accuracy = Math.RandomFloat(diff.AccuracyMin, diff.AccuracyMax);
		float distanceToTarget = m_Bot.DistanceToTarget();
		
		// Precisao diminui com a distancia
		float distancePenalty = 0;
		if (distanceToTarget > 50.0)
			distancePenalty = (distanceToTarget - 50.0) / 200.0;
		
		accuracy = Math.Clamp(accuracy - distancePenalty, 0.0, 1.0);
		
		bool hit = Math.RandomFloat01() <= accuracy;
		
		// Calcular direcao do tiro
		vector botPos = m_Bot.GetPosition() + Vector(0, 1.5, 0);
		vector targetPos = target.GetPosition() + Vector(0, 1.0, 0);
		vector direction = (targetPos - botPos).Normalized();
		
		// Aplicar spread (dispersao)
		float spreadRad = diff.AimSpread * Math.DEG2RAD;
		direction[0] = direction[0] + Math.RandomFloat(-spreadRad, spreadRad);
		direction[1] = direction[1] + Math.RandomFloat(-spreadRad * 0.5, spreadRad * 0.5);
		direction[2] = direction[2] + Math.RandomFloat(-spreadRad, spreadRad);
		direction.Normalize();
		
		// Aplicar dano se acertou
		float damage = 0;
		
		if (hit)
		{
			m_ShotsHit++;
			damage = CalculateRangedDamage(diff, distanceToTarget);
			ApplyDamageToPlayer(target, damage);
		}
		
		string targetName = "Unknown";
		if (target.GetIdentity())
			targetName = target.GetIdentity().GetName();
		
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogCombat)
		{
			ABLogger.LogCombatShot(m_Bot.GetName(), targetName, hit, damage);
		}
		
		// Efeito sonoro do tiro (via animacao do bot)
		PlayShootAnimation(botEntity);
	}
	
	protected float CalculateRangedDamage(ABDifficultyConfig diff, float distance)
	{
		float baseDamage = 25.0;
		
		// Dano diminui com distancia
		float distFactor = 1.0;
		if (distance > 30.0)
			distFactor = Math.Clamp(1.0 - ((distance - 30.0) / 150.0), 0.3, 1.0);
		
		float finalDamage = baseDamage * diff.DamageMultiplier * distFactor;
		
		// Hit zones aleatorias afetam o dano
		float hitZoneMultiplier = GetRandomHitZoneMultiplier();
		finalDamage *= hitZoneMultiplier;
		
		return finalDamage;
	}
	
	protected float GetRandomHitZoneMultiplier()
	{
		float roll = Math.RandomFloat01();
		
		if (roll < 0.05)      // 5% chance headshot
			return 4.0;
		else if (roll < 0.35) // 30% chance torso
			return 1.0;
		else if (roll < 0.65) // 30% chance arms
			return 0.6;
		else                  // 35% chance legs
			return 0.5;
	}
	
	protected void ApplyDamageToPlayer(PlayerBase target, float damage)
	{
		if (!target || !target.IsAlive())
			return;
		
		// Selecionar zona de dano aleatoria
		string damageZone = GetRandomDamageZone();
		
		target.ProcessDirectDamage(DT_CUSTOM, m_Bot.GetEntity(), damageZone, "Bullet_556x45", "0 0 0", damage);
	}
	
	protected string GetRandomDamageZone()
	{
		float roll = Math.RandomFloat01();
		
		if (roll < 0.05)
			return "Head";
		else if (roll < 0.35)
			return "Torso";
		else if (roll < 0.50)
			return "LeftArm";
		else if (roll < 0.65)
			return "RightArm";
		else if (roll < 0.82)
			return "LeftLeg";
		else
			return "RightLeg";
	}
	
	protected void PlayShootAnimation(PlayerBase botEntity)
	{
		if (!botEntity)
			return;
		
		// Forcar animacao de tiro no bot
		HumanCommandMove moveCmd = botEntity.GetCommand_Move();
		if (moveCmd)
		{
			// Bot fica ereto ao atirar
			moveCmd.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
		}
	}
	
	// --- MELEE COMBAT (FACA) ---
	
	void MeleeAttack(PlayerBase target)
	{
		if (!m_Bot || !m_Bot.IsAlive() || !target || !target.IsAlive())
			return;
		
		m_MeleeTimer += 1.0;
		
		if (m_MeleeTimer < m_MeleeCooldown)
			return;
		
		m_MeleeTimer = 0;
		
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		if (!diff)
			return;
		
		float dist = m_Bot.DistanceToTarget();
		
		float meleeRange = 3.0;
		if (ABConfig.s_Settings)
			meleeRange = ABConfig.s_Settings.MeleeEngageDistance;
		
		if (dist > meleeRange)
			return;
		
		// Calcular hit/miss com base na precisao + brutalidade
		float hitChance = Math.Clamp(diff.AccuracyMax + (diff.Brutality * 0.2), 0.0, 1.0);
		bool hit = Math.RandomFloat01() <= hitChance;
		
		float damage = 0;
		
		if (hit)
		{
			damage = diff.MeleeDamage;
			
			// Aplicar dano melee
			string meleeZone = GetRandomDamageZone();
			target.ProcessDirectDamage(DT_CUSTOM, m_Bot.GetEntity(), meleeZone, "MeleeFist", "0 0 0", damage);
		}
		
		string targetName = "Unknown";
		if (target.GetIdentity())
			targetName = target.GetIdentity().GetName();
		
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogCombat)
		{
			ABLogger.LogCombatMelee(m_Bot.GetName(), targetName, hit, damage);
		}
		
		// Animacao de ataque melee
		PlayMeleeAnimation(m_Bot.GetEntity());
	}
	
	protected void PlayMeleeAnimation(PlayerBase botEntity)
	{
		if (!botEntity)
			return;
		
		HumanCommandMove moveCmd = botEntity.GetCommand_Move();
		if (moveCmd)
		{
			moveCmd.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
		}
	}
	
	// --- Stats ---
	
	int GetShotsFired()
	{
		return m_ShotsFired;
	}
	
	int GetShotsHit()
	{
		return m_ShotsHit;
	}
	
	float GetAccuracy()
	{
		if (m_ShotsFired == 0)
			return 0;
		return m_ShotsHit / m_ShotsFired;
	}
};
