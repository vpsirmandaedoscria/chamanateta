// AloneZ BOTS - Bot Group Management
// Gerenciamento de grupos de bots por spawn configuravel

class ABBotGroup
{
	protected string m_GroupName;
	protected string m_Difficulty;
	protected ref array<ref ABBot> m_Members;
	protected vector m_SpawnPosition;
	protected int m_MaxSize;
	protected bool m_RespawnEnabled;
	protected float m_RespawnTime;
	protected float m_RespawnTimer;
	protected bool m_NeedsRespawn;
	protected int m_DeadCount;
	protected PlayerBase m_GroupTarget;
	protected ref ABSpawnPointConfig m_SpawnConfig;
	
	void ABBotGroup(string groupName, string difficulty, vector spawnPos, int maxSize)
	{
		m_GroupName = groupName;
		m_Difficulty = difficulty;
		m_Members = new array<ref ABBot>();
		m_SpawnPosition = spawnPos;
		m_MaxSize = maxSize;
		m_RespawnEnabled = true;
		m_RespawnTime = 300.0;
		m_RespawnTimer = 0;
		m_NeedsRespawn = false;
		m_DeadCount = 0;
		m_GroupTarget = null;
		
		ABLogger.LogGroupCreated(groupName, maxSize, difficulty);
	}
	
	void SetSpawnConfig(ABSpawnPointConfig config)
	{
		m_SpawnConfig = config;
		if (config)
		{
			m_RespawnEnabled = config.RespawnEnabled;
			m_RespawnTime = config.RespawnTime;
		}
	}
	
	void AddMember(ABBot bot)
	{
		if (!bot)
			return;
		
		m_Members.Insert(bot);
		bot.SetGroup(this);
	}
	
	void RemoveMember(ABBot bot)
	{
		if (!bot)
			return;
		
		int idx = m_Members.Find(bot);
		if (idx >= 0)
			m_Members.Remove(idx);
	}
	
	void Update(float deltaTime)
	{
		// Atualizar todos os bots do grupo
		for (int i = m_Members.Count() - 1; i >= 0; i--)
		{
			ABBot bot = m_Members[i];
			if (bot)
			{
				if (bot.IsAlive())
				{
					bot.Update(deltaTime);
				}
			}
		}
		
		// Verificar respawn
		if (m_NeedsRespawn && m_RespawnEnabled)
		{
			m_RespawnTimer += deltaTime;
			
			if (m_RespawnTimer >= m_RespawnTime)
			{
				m_NeedsRespawn = false;
				m_RespawnTimer = 0;
				ABLogger.LogRespawn(m_GroupName, m_SpawnPosition);
				RequestRespawn();
			}
		}
	}
	
	void OnBotDied(ABBot bot)
	{
		m_DeadCount++;
		
		ABLogger.Log("INFO", "GROUP", "Bot morreu no grupo '" + m_GroupName + "' | Mortos: " + m_DeadCount.ToString() + "/" + m_Members.Count().ToString());
		
		// Verificar se todos morreram
		if (GetAliveCount() == 0)
		{
			ABLogger.Log("INFO", "GROUP", "Todos os bots do grupo '" + m_GroupName + "' morreram! Respawn em " + m_RespawnTime.ToString() + "s");
			m_NeedsRespawn = true;
			m_RespawnTimer = 0;
		}
	}
	
	void AlertGroup(PlayerBase target, ABBot alerter)
	{
		if (!target)
			return;
		
		m_GroupTarget = target;
		
		string targetName = "Unknown";
		if (target.GetIdentity())
			targetName = target.GetIdentity().GetName();
		
		ABLogger.Log("INFO", "GROUP", "Grupo '" + m_GroupName + "' alertado sobre player '" + targetName + "' por bot '" + alerter.GetName() + "'");
		
		// Alertar todos os membros do grupo
		foreach (ABBot bot : m_Members)
		{
			if (!bot || !bot.IsAlive())
				continue;
			
			if (bot == alerter)
				continue;
			
			// Se o bot nao tem alvo, dar o alvo do grupo
			if (!bot.GetTarget())
			{
				bot.SetTarget(target);
				
				float dist = bot.DistanceToTarget();
				ABDifficultyConfig diff = bot.GetDifficultyConfig();
				
				float stealthDist = 50.0;
				if (diff)
					stealthDist = diff.StealthDistance;
				
				if (dist > stealthDist)
				{
					bot.SetState(ABBotState.STEALTH);
				}
				else
				{
					bot.SetState(ABBotState.COMBAT_RANGED);
				}
			}
		}
	}
	
	protected void RequestRespawn()
	{
		// Solicitar respawn ao SpawnManager
		ABSpawnManager.RequestGroupRespawn(this);
	}
	
	// --- Getters ---
	
	string GetGroupName()
	{
		return m_GroupName;
	}
	
	string GetDifficulty()
	{
		return m_Difficulty;
	}
	
	vector GetSpawnPosition()
	{
		return m_SpawnPosition;
	}
	
	int GetMaxSize()
	{
		return m_MaxSize;
	}
	
	int GetAliveCount()
	{
		int count = 0;
		foreach (ABBot bot : m_Members)
		{
			if (bot && bot.IsAlive())
				count++;
		}
		return count;
	}
	
	int GetTotalCount()
	{
		return m_Members.Count();
	}
	
	int GetDeadCount()
	{
		return m_DeadCount;
	}
	
	array<ref ABBot> GetMembers()
	{
		return m_Members;
	}
	
	ABSpawnPointConfig GetSpawnConfig()
	{
		return m_SpawnConfig;
	}
	
	bool IsRespawnEnabled()
	{
		return m_RespawnEnabled;
	}
	
	void SetRespawnEnabled(bool enabled)
	{
		m_RespawnEnabled = enabled;
	}
	
	void SetRespawnTime(float time)
	{
		m_RespawnTime = time;
	}
	
	PlayerBase GetGroupTarget()
	{
		return m_GroupTarget;
	}
	
	void ClearGroupTarget()
	{
		m_GroupTarget = null;
	}
	
	void CleanupDeadBots()
	{
		for (int i = m_Members.Count() - 1; i >= 0; i--)
		{
			ABBot bot = m_Members[i];
			if (bot && !bot.IsAlive())
			{
				PlayerBase entity = bot.GetEntity();
				if (entity)
				{
					GetGame().ObjectDelete(entity);
				}
				m_Members.Remove(i);
			}
		}
	}
};
