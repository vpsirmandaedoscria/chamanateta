class ABBotCombat
{
	protected ABBot m_Bot;
	protected float m_MeleeTimer;
	protected float m_MeleeCooldown;
	protected int m_ShotsFired;
	protected int m_ShotsHit;
	protected bool m_WeaponRaised;
	protected float m_RaiseTimer;
	
	void ABBotCombat(ABBot bot)
	{
		m_Bot = bot;
		m_MeleeTimer = 0;
		m_MeleeCooldown = 1.5;
		m_ShotsFired = 0;
		m_ShotsHit = 0;
		m_WeaponRaised = false;
		m_RaiseTimer = 0;
	}
	
	void RaiseWeapon(PlayerBase botEntity)
	{
		if (!botEntity)
			return;
		
		HumanInputController hic = botEntity.GetInputController();
		if (hic)
		{
			hic.OverrideRaise(true, true);
			m_WeaponRaised = true;
		}
	}
	
	void LowerWeapon(PlayerBase botEntity)
	{
		if (!botEntity)
			return;
		
		HumanInputController hic = botEntity.GetInputController();
		if (hic)
		{
			hic.OverrideRaise(true, false);
			m_WeaponRaised = false;
		}
	}
	
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
		
		if (!m_WeaponRaised)
		{
			RaiseWeapon(botEntity);
			m_RaiseTimer = 0;
			return;
		}
		
		m_RaiseTimer += 1.0;
		if (m_RaiseTimer < 0.5)
			return;
		
		m_ShotsFired++;
		
		float accuracy = Math.RandomFloat(diff.AccuracyMin, diff.AccuracyMax);
		float distanceToTarget = m_Bot.DistanceToTarget();
		
		float distancePenalty = 0;
		if (distanceToTarget > 50.0)
			distancePenalty = (distanceToTarget - 50.0) / 200.0;
		
		accuracy = Math.Clamp(accuracy - distancePenalty, 0.0, 1.0);
		
		bool hit = Math.RandomFloat01() <= accuracy;
		
		vector botPos = m_Bot.GetPosition() + Vector(0, 1.5, 0);
		vector targetPos = target.GetPosition() + Vector(0, 1.0, 0);
		vector direction = (targetPos - botPos).Normalized();
		
		float spreadRad = diff.AimSpread * Math.DEG2RAD;
		direction[0] = direction[0] + Math.RandomFloat(-spreadRad, spreadRad);
		direction[1] = direction[1] + Math.RandomFloat(-spreadRad * 0.5, spreadRad * 0.5);
		direction[2] = direction[2] + Math.RandomFloat(-spreadRad, spreadRad);
		direction.Normalize();
		
		float damage = 0;
		
		if (hit)
		{
			m_ShotsHit++;
			damage = CalculateRangedDamage(diff, distanceToTarget);
			ApplyDamageToPlayer(target, damage);
		}
		
		TryFireWeapon(botEntity);
		
		string targetName = "Unknown";
		if (target.GetIdentity())
			targetName = target.GetIdentity().GetName();
		
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogCombat)
		{
			ABLogger.LogCombatShot(m_Bot.GetName(), targetName, hit, damage);
		}
	}
	
	protected void TryFireWeapon(PlayerBase botEntity)
	{
		if (!botEntity)
			return;
		
		Weapon_Base weapon = Weapon_Base.Cast(botEntity.GetItemInHands());
		if (!weapon)
			return;
		
		int mi = weapon.GetCurrentMuzzle();
		
		if (weapon.IsChamberFull(mi))
		{
			weapon.ProcessWeaponEvent(new WeaponEventTrigger(botEntity));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CycleAction, 300, false, botEntity);
		}
		else
		{
			Magazine mag = Magazine.Cast(weapon.GetMagazine(mi));
			if (mag && mag.GetAmmoCount() > 0)
			{
				weapon.ProcessWeaponEvent(new WeaponEventMechanism(botEntity));
			}
			else
			{
				TryReload(botEntity, weapon);
			}
		}
	}
	
	protected void CycleAction(PlayerBase botEntity)
	{
		if (!botEntity)
			return;
		
		Weapon_Base weapon = Weapon_Base.Cast(botEntity.GetItemInHands());
		if (!weapon)
			return;
		
		int mi = weapon.GetCurrentMuzzle();
		if (weapon.IsChamberEmpty(mi))
		{
			weapon.ProcessWeaponEvent(new WeaponEventMechanism(botEntity));
		}
	}
	
	protected void TryReload(PlayerBase botEntity, Weapon_Base weapon)
	{
		if (!botEntity || !weapon)
			return;
		
		WeaponManager wm = botEntity.GetWeaponManager();
		if (!wm)
			return;
		
		int mi = weapon.GetCurrentMuzzle();
		Magazine mag = Magazine.Cast(weapon.GetMagazine(mi));
		
		if (mag && mag.GetAmmoCount() > 0)
			return;
		
		Magazine spareMag = null;
		EntityAI attachment;
		int attCount = botEntity.GetInventory().GetAttachmentSlotsCount();
		for (int i = 0; i < attCount; i++)
		{
			EntityAI att = botEntity.GetInventory().FindAttachment(i);
			if (!att)
				continue;
			
			Magazine testMag = Magazine.Cast(att);
			if (testMag && testMag.GetAmmoCount() > 0 && weapon.CanChamberFromMag(mi, testMag))
			{
				spareMag = testMag;
				break;
			}
		}
		
		if (spareMag && wm.CanAttachMagazine(weapon, spareMag))
		{
			wm.AttachMagazine(spareMag);
		}
	}
	
	protected float CalculateRangedDamage(ABDifficultyConfig diff, float distance)
	{
		float baseDamage = 25.0;
		
		float distFactor = 1.0;
		if (distance > 30.0)
			distFactor = Math.Clamp(1.0 - ((distance - 30.0) / 150.0), 0.3, 1.0);
		
		float finalDamage = baseDamage * diff.DamageMultiplier * distFactor;
		
		float hitZoneMultiplier = GetRandomHitZoneMultiplier();
		finalDamage = finalDamage * hitZoneMultiplier;
		
		return finalDamage;
	}
	
	protected float GetRandomHitZoneMultiplier()
	{
		float roll = Math.RandomFloat01();
		
		if (roll < 0.05)
			return 4.0;
		else if (roll < 0.35)
			return 1.0;
		else if (roll < 0.65)
			return 0.6;
		else
			return 0.5;
	}
	
	protected void ApplyDamageToPlayer(PlayerBase target, float damage)
	{
		if (!target || !target.IsAlive())
			return;
		
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
		
		PlayerBase botEntity = m_Bot.GetEntity();
		if (!botEntity)
			return;
		
		float dist = m_Bot.DistanceToTarget();
		
		float meleeRange = 3.0;
		if (ABConfig.s_Settings)
			meleeRange = ABConfig.s_Settings.MeleeEngageDistance;
		
		if (dist > meleeRange)
			return;
		
		if (!m_WeaponRaised)
		{
			RaiseWeapon(botEntity);
			return;
		}
		
		HumanCommandMove moveCmd = botEntity.GetCommand_Move();
		if (moveCmd)
		{
			moveCmd.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
		}
		
		vector targetPos = target.GetPosition();
		vector hitPos = targetPos + Vector(0, 1.0, 0);
		float attackDist = 1.0;
		if (dist < 1.5)
			attackDist = 0.5;
		
		bool isStab = false;
		EntityAI itemInHands = botEntity.GetItemInHands();
		if (itemInHands && itemInHands.IsWeapon())
			isStab = true;
		
		botEntity.StartCommand_Melee2(target, isStab, attackDist, hitPos);
		
		float hitChance = Math.Clamp(diff.AccuracyMax + (diff.Brutality * 0.2), 0.0, 1.0);
		bool hit = Math.RandomFloat01() <= hitChance;
		
		float damage = 0;
		
		if (hit)
		{
			damage = diff.MeleeDamage;
			string meleeZone = GetRandomDamageZone();
			target.ProcessDirectDamage(DT_CUSTOM, botEntity, meleeZone, "MeleeFist", "0 0 0", damage);
		}
		
		string targetName = "Unknown";
		if (target.GetIdentity())
			targetName = target.GetIdentity().GetName();
		
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogCombat)
		{
			ABLogger.LogCombatMelee(m_Bot.GetName(), targetName, hit, damage);
		}
	}
	
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
