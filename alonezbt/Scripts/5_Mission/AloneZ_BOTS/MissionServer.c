modded class MissionServer
{
	protected ref ABBotManager m_ABBotManager;
	
	override void OnInit()
	{
		super.OnInit();
		
		m_ABBotManager = ABBotManager.GetInstance();
		
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
