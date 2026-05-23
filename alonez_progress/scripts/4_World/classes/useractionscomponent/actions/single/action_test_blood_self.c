modded class ActionTestBloodSelf
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		PlayerBase pb = PlayerBase.Cast(action_data.m_Player);
		ALZAgentReportSrv.SendTo(pb);
	}
}
