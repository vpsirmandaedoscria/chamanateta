class ABBotManager
{
	static ref ABBotManager s_Instance;
	
	protected bool m_Initialized;
	protected float m_UpdateTimer;
	protected float m_StatusLogTimer;
	protected float m_StatusLogInterval;
	
	void ABBotManager()
	{
		m_Initialized = false;
		m_UpdateTimer = 0;
		m_StatusLogTimer = 0;
		m_StatusLogInterval = 300.0;
	}
	
	static ABBotManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new ABBotManager();
		return s_Instance;
	}
	
	void Init()
	{
		if (m_Initialized)
			return;
		
		ABLogger.Log("INFO", "BOT_MGR", "==================================");
		ABLogger.Log("INFO", "BOT_MGR", "  AloneZ BOTS v1.0.0 Iniciando");
		ABLogger.Log("INFO", "BOT_MGR", "==================================");
		
		ABConfig.Load();
		
		if (!ABConfig.s_Settings || !ABConfig.s_Settings.Enabled)
		{
			ABLogger.Log("WARN", "BOT_MGR", "AloneZ BOTS esta DESABILITADO nas configuracoes!");
			return;
		}
		
		ABSpawnManager.Init();
		ABSpawnManager.SpawnAllConfigured();
		
		m_Initialized = true;
		
		ABLogger.Log("INFO", "BOT_MGR", "AloneZ BOTS inicializado com sucesso!");
		ABLogger.Log("INFO", "BOT_MGR", "Total de bots: " + ABSpawnManager.GetTotalBotCount().ToString());
		ABLogger.Log("INFO", "BOT_MGR", "Total de grupos: " + ABSpawnManager.GetGroupCount().ToString());
	}
	
	void Update(float deltaTime)
	{
		if (!m_Initialized)
			return;
		
		m_UpdateTimer += deltaTime;
		m_StatusLogTimer += deltaTime;
		
		ABSpawnManager.Update(deltaTime);
		
		if (m_StatusLogTimer >= m_StatusLogInterval)
		{
			m_StatusLogTimer = 0;
			LogStatus();
		}
	}
	
	void LogStatus()
	{
		int totalBots = ABSpawnManager.GetTotalBotCount();
		int aliveBots = ABSpawnManager.GetAliveBotCount();
		int groups = ABSpawnManager.GetGroupCount();
		
		ABLogger.Log("INFO", "STATUS", "=== Status AloneZ BOTS ===");
		ABLogger.Log("INFO", "STATUS", "Bots Vivos: " + aliveBots.ToString() + "/" + totalBots.ToString());
		ABLogger.Log("INFO", "STATUS", "Grupos: " + groups.ToString());
		ABLogger.Log("INFO", "STATUS", "========================");
	}
	
	void Shutdown()
	{
		ABLogger.Log("INFO", "BOT_MGR", "AloneZ BOTS encerrando...");
		
		ABSpawnManager.Cleanup();
		ABLogger.Close();
		
		m_Initialized = false;
	}
	
	void ReloadConfig()
	{
		ABLogger.Log("INFO", "BOT_MGR", "Recarregando configuracoes...");
		
		ABSpawnManager.Cleanup();
		ABConfig.Reload();
		ABSpawnManager.Init();
		ABSpawnManager.SpawnAllConfigured();
		
		ABLogger.Log("INFO", "BOT_MGR", "Configuracoes recarregadas! Bots: " + ABSpawnManager.GetTotalBotCount().ToString());
	}
	
	bool IsInitialized()
	{
		return m_Initialized;
	}
};
