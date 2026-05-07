// AloneZ BOTS - Mission Server Integration
// Hooks into the DayZ mission lifecycle to manage bots

modded class MissionServer
{
	protected ref ABBotManager m_ABBotManager;
	
	override void OnInit()
	{
		super.OnInit();
		
		// Inicializar o gerenciador de bots
		m_ABBotManager = ABBotManager.GetInstance();
		
		// Atrasar inicializacao para garantir que o mapa esta carregado
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitABBots, 5000, false);
	}
	
	void InitABBots()
	{
		if (m_ABBotManager)
		{
			m_ABBotManager.Init();
		}
	}
	
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		
		if (m_ABBotManager)
		{
			m_ABBotManager.Update(timeslice);
		}
	}
	
	override void OnMissionFinish()
	{
		if (m_ABBotManager)
		{
			m_ABBotManager.Shutdown();
		}
		
		super.OnMissionFinish();
	}
};
