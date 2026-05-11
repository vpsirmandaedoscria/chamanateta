enum ABBotState
{
	IDLE,
	PATROLLING,
	DETECTING,
	STEALTH,
	COMBAT_RANGED,
	COMBAT_MELEE,
	DEAD
};

class ABBot
{
	protected PlayerBase m_BotEntity;
	protected string m_Name;
	protected string m_Difficulty;
	protected string m_GroupName;
	protected ABBotState m_State;
	protected ABBotState m_PreviousState;
	
	protected ref ABBotBrain m_Brain;
	protected ref ABBotDetection m_Detection;
	protected ref ABBotCombat m_Combat;
	protected ref ABBotPatrol m_Patrol;
	protected ref ABDifficultyConfig m_DiffConfig;
	
	protected vector m_SpawnPosition;
	protected PlayerBase m_CurrentTarget;
	protected bool m_IsAlive;
	protected float m_Health;
	protected float m_MaxHealth;
	
	protected float m_TimeSinceLastUpdate;
	protected float m_TimeSinceLastPositionLog;
	protected float m_TimeSinceStateChange;
	protected float m_StealthTimer;
	
	protected ref ABBotGroup m_Group;
	protected ref ABBotMoveCommand m_MoveCommand;
	protected bool m_MoveCommandStarted;
	
	void ABBot(PlayerBase entity, string name, string difficulty, vector spawnPos, string groupName)
	{
		m_BotEntity = entity;
		m_Name = name;
		m_Difficulty = difficulty;
		m_SpawnPosition = spawnPos;
		m_GroupName = groupName;
		m_State = ABBotState.IDLE;
		m_PreviousState = ABBotState.IDLE;
		m_IsAlive = true;
		
		m_DiffConfig = ABConfig.GetDifficulty(difficulty);
		
		if (m_DiffConfig)
		{
			m_MaxHealth = 100.0 * m_DiffConfig.HealthMultiplier;
		}
		else
		{
			m_MaxHealth = 100.0;
		}
		m_Health = m_MaxHealth;
		
		m_Detection = new ABBotDetection(this);
		m_Combat = new ABBotCombat(this);
		m_Patrol = new ABBotPatrol(this);
		m_Brain = new ABBotBrain(this);
		
		m_TimeSinceLastUpdate = 0;
		m_TimeSinceLastPositionLog = 0;
		m_TimeSinceStateChange = 0;
		m_StealthTimer = 0;
		m_MoveCommandStarted = false;
		
		ABLogger.LogInit(m_Name, "default", m_Difficulty);
		ABLogger.LogSpawn(m_Name, m_Difficulty, spawnPos, m_GroupName);
	}
	
	void ~ABBot()
	{
		if (m_BotEntity && m_IsAlive)
		{
			ABLogger.LogDespawn(m_Name, "Destrutor chamado");
		}
	}
	
	void Update(float deltaTime)
	{
		if (!m_IsAlive || !m_BotEntity)
			return;
		
		m_TimeSinceLastUpdate += deltaTime;
		m_TimeSinceLastPositionLog += deltaTime;
		m_TimeSinceStateChange += deltaTime;
		
		float updateInterval = 1.0;
		if (ABConfig.s_Settings)
			updateInterval = ABConfig.s_Settings.BotUpdateInterval;
		
		if (m_TimeSinceLastUpdate < updateInterval)
			return;
		
		m_TimeSinceLastUpdate = 0;
		
		float posLogInterval = 30.0;
		if (ABConfig.s_Settings)
			posLogInterval = ABConfig.s_Settings.PositionLogInterval;
		
		if (m_TimeSinceLastPositionLog >= posLogInterval)
		{
			m_TimeSinceLastPositionLog = 0;
			if (ABConfig.s_Settings && ABConfig.s_Settings.LogPositions)
			{
				ABLogger.LogPosition(m_Name, GetPosition());
			}
		}
		
		if (m_Brain)
			m_Brain.Think(deltaTime);
	}
	
	void SetState(ABBotState newState)
	{
		if (newState == m_State)
			return;
		
		m_PreviousState = m_State;
		m_State = newState;
		m_TimeSinceStateChange = 0;
		
		string fromStr = GetStateName(m_PreviousState);
		string toStr = GetStateName(newState);
		ABLogger.LogStateChange(m_Name, fromStr, toStr);
	}
	
	ABBotState GetState()
	{
		return m_State;
	}
	
	static string GetStateName(ABBotState state)
	{
		switch (state)
		{
			case ABBotState.IDLE: return "IDLE";
			case ABBotState.PATROLLING: return "PATROLLING";
			case ABBotState.DETECTING: return "DETECTING";
			case ABBotState.STEALTH: return "STEALTH";
			case ABBotState.COMBAT_RANGED: return "COMBAT_RANGED";
			case ABBotState.COMBAT_MELEE: return "COMBAT_MELEE";
			case ABBotState.DEAD: return "DEAD";
		}
		return "UNKNOWN";
	}
	
	PlayerBase GetEntity()
	{
		return m_BotEntity;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	string GetDifficulty()
	{
		return m_Difficulty;
	}
	
	string GetGroupName()
	{
		return m_GroupName;
	}
	
	ABDifficultyConfig GetDifficultyConfig()
	{
		return m_DiffConfig;
	}
	
	ABBotDetection GetDetection()
	{
		return m_Detection;
	}
	
	ABBotCombat GetCombat()
	{
		return m_Combat;
	}
	
	ABBotPatrol GetPatrol()
	{
		return m_Patrol;
	}
	
	ABBotBrain GetBrain()
	{
		return m_Brain;
	}
	
	ABBotGroup GetGroup()
	{
		return m_Group;
	}
	
	void SetGroup(ABBotGroup group)
	{
		m_Group = group;
	}
	
	vector GetPosition()
	{
		if (m_BotEntity)
			return m_BotEntity.GetPosition();
		return m_SpawnPosition;
	}
	
	vector GetSpawnPosition()
	{
		return m_SpawnPosition;
	}
	
	bool IsAlive()
	{
		return m_IsAlive;
	}
	
	float GetHealth()
	{
		return m_Health;
	}
	
	float GetMaxHealth()
	{
		return m_MaxHealth;
	}
	
	void SetTarget(PlayerBase target)
	{
		m_CurrentTarget = target;
	}
	
	PlayerBase GetTarget()
	{
		return m_CurrentTarget;
	}
	
	void ClearTarget()
	{
		m_CurrentTarget = null;
	}
	
	void TakeDamage(float damage, string source)
	{
		if (!m_IsAlive)
			return;
		
		float actualDamage = damage;
		if (m_DiffConfig)
			actualDamage = damage * m_DiffConfig.DamageReceivedMultiplier;
		
		m_Health = m_Health - actualDamage;
		
		ABLogger.Log("INFO", "DAMAGE", "Bot '" + m_Name + "' recebeu " + actualDamage.ToString() + " de dano de '" + source + "' | HP: " + m_Health.ToString() + "/" + m_MaxHealth.ToString());
		
		if (m_Health <= 0)
		{
			OnDeath(source);
		}
	}
	
	void OnDeath(string killerName)
	{
		if (!m_IsAlive)
			return;
		
		m_IsAlive = false;
		SetState(ABBotState.DEAD);
		
		ABLogger.LogDeath(m_Name, killerName, GetPosition());
		
		DropLoot();
		
		if (m_Group)
			m_Group.OnBotDied(this);
	}
	
	void DropLoot()
	{
		ABLootTable lootTable = ABConfig.GetLootTable(m_Difficulty);
		if (!lootTable || !lootTable.Items)
			return;
		
		for (int i = 0; i < lootTable.Items.Count(); i++)
		{
			ABLootItem lootItem = lootTable.Items[i];
			float roll = Math.RandomFloat01();
			if (roll > lootItem.DropChance)
				continue;
			
			int quantity = Math.RandomIntInclusive(lootItem.QuantityMin, lootItem.QuantityMax);
			
			if (m_BotEntity)
			{
				vector dropPos = GetPosition() + Vector(Math.RandomFloat(-1.0, 1.0), 0, Math.RandomFloat(-1.0, 1.0));
				
				EntityAI item = EntityAI.Cast(GetGame().CreateObjectEx(lootItem.ClassName, dropPos, ECE_PLACE_ON_SURFACE));
				if (item)
				{
					Magazine mag = Magazine.Cast(item);
					if (mag)
					{
						mag.ServerSetAmmoCount(quantity);
					}
					
					ABLogger.LogLoot(m_Name, lootItem.ClassName, quantity);
				}
			}
		}
	}
	
	void StartMoveCommand()
	{
		if (!m_BotEntity || m_MoveCommandStarted)
			return;
		
		m_MoveCommand = new ABBotMoveCommand();
		m_BotEntity.StartCommand_Script(m_MoveCommand);
		m_MoveCommandStarted = true;
	}
	
	void MoveTo(vector targetPos, float speedMultiplier)
	{
		if (!m_BotEntity || !m_IsAlive)
			return;
		
		if (!m_MoveCommandStarted)
			StartMoveCommand();
		
		vector currentPos = GetPosition();
		vector direction = targetPos - currentPos;
		direction[1] = 0;
		float dist = direction.Length();
		
		if (dist < 0.3)
		{
			StopMovement();
			return;
		}
		
		direction.Normalize();
		
		float yaw = direction.VectorToAngles()[0];
		m_BotEntity.SetOrientation(Vector(yaw, 0, 0));
		
		float baseSpeed = 1.8;
		if (speedMultiplier < 0.5)
			baseSpeed = 0.8;
		else if (speedMultiplier < 1.0)
			baseSpeed = 1.8;
		else
			baseSpeed = 3.5;
		
		float speed = baseSpeed * speedMultiplier;
		if (speed > 5.0)
			speed = 5.0;
		
		if (m_MoveCommand)
		{
			m_MoveCommand.SetMovement(direction, speed);
		}
	}
	
	void StopMovement()
	{
		if (m_MoveCommand)
		{
			m_MoveCommand.Stop();
		}
	}
	
	void LookAt(vector targetPos)
	{
		if (!m_BotEntity || !m_IsAlive)
			return;
		
		vector direction = targetPos - GetPosition();
		direction.Normalize();
		float yaw = direction.VectorToAngles()[0];
		m_BotEntity.SetOrientation(Vector(yaw, 0, 0));
	}
	
	float DistanceTo(vector pos)
	{
		return vector.Distance(GetPosition(), pos);
	}
	
	float DistanceToTarget()
	{
		if (!m_CurrentTarget)
			return -1;
		return vector.Distance(GetPosition(), m_CurrentTarget.GetPosition());
	}
};
