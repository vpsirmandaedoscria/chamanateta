class ABBotMoveCommand extends HumanCommandScript
{
	protected float m_Speed;
	protected vector m_Direction;
	protected bool m_Active;
	
	void ABBotMoveCommand()
	{
		m_Speed = 0;
		m_Direction = vector.Zero;
		m_Active = false;
	}
	
	void SetMovement(vector direction, float speed)
	{
		m_Direction = direction;
		m_Direction[1] = 0;
		if (m_Direction.Length() > 0.01)
			m_Direction.Normalize();
		m_Speed = speed;
		m_Active = (speed > 0.01);
	}
	
	void Stop()
	{
		m_Speed = 0;
		m_Active = false;
		m_Direction = vector.Zero;
	}
	
	bool IsActive()
	{
		return m_Active;
	}
	
	float GetSpeed()
	{
		return m_Speed;
	}
	
	override void PrePhys_SetTranslation(inout vector pTranslation)
	{
		if (m_Active && m_Speed > 0)
		{
			pTranslation[0] = m_Direction[0] * m_Speed * 0.02;
			pTranslation[1] = 0;
			pTranslation[2] = m_Direction[2] * m_Speed * 0.02;
		}
		else
		{
			pTranslation = vector.Zero;
		}
	}
};
