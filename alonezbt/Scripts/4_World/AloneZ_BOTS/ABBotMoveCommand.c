class ABBotMoveCommand
{
	protected PlayerBase m_Entity;
	protected float m_TargetSpeed;
	protected float m_MovementAngle;
	protected bool m_Active;
	protected bool m_TimerStarted;
	protected bool m_WeaponRaised;
	
	void ABBotMoveCommand(PlayerBase entity)
	{
		m_Entity = entity;
		m_TargetSpeed = 0;
		m_MovementAngle = 0;
		m_Active = false;
		m_TimerStarted = false;
		m_WeaponRaised = false;
	}
	
	void ~ABBotMoveCommand()
	{
		StopTimer();
	}
	
	void StartTimer()
	{
		if (m_TimerStarted)
			return;
		
		m_TimerStarted = true;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnFrame, 33, true);
	}
	
	void StopTimer()
	{
		if (!m_TimerStarted)
			return;
		
		m_TimerStarted = false;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(OnFrame);
	}
	
	void SetMovement(float speed, float angle)
	{
		m_TargetSpeed = speed;
		m_MovementAngle = angle;
		m_Active = (speed > 0.01);
		
		if (m_Active && !m_TimerStarted)
			StartTimer();
	}
	
	void Stop()
	{
		m_TargetSpeed = 0;
		m_MovementAngle = 0;
		m_Active = false;
		
		if (m_Entity)
		{
			HumanInputController hic = m_Entity.GetInputController();
			if (hic)
			{
				hic.OverrideMovementSpeed(true, 0);
			}
		}
	}
	
	bool IsActive()
	{
		return m_Active;
	}
	
	void SetWeaponRaised(bool raised)
	{
		m_WeaponRaised = raised;
		if (raised && !m_TimerStarted)
			StartTimer();
	}
	
	bool IsWeaponRaised()
	{
		return m_WeaponRaised;
	}
	
	void OnFrame()
	{
		if (!m_Entity)
			return;
		
		if (!m_Active && !m_WeaponRaised)
			return;
		
		HumanInputController hic = m_Entity.GetInputController();
		if (!hic)
			return;
		
		if (m_Active)
		{
			hic.OverrideMovementSpeed(true, m_TargetSpeed);
			hic.OverrideMovementAngle(true, m_MovementAngle);
		}
		
		if (m_WeaponRaised)
		{
			hic.OverrideRaise(true, true);
		}
		
		HumanCommandMove moveCmd = m_Entity.GetCommand_Move();
		if (moveCmd)
		{
			moveCmd.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
		}
	}
};
