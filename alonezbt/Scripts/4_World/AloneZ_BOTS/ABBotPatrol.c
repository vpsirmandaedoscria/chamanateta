// AloneZ BOTS - Patrol/Route System
// Rotas automaticas pelo mapa em ciclo loop

class ABBotPatrol
{
	protected ABBot m_Bot;
	protected ref array<vector> m_Waypoints;
	protected ref array<float> m_WaitTimes;
	protected int m_CurrentWaypointIndex;
	protected bool m_IsWaiting;
	protected float m_WaitTimer;
	protected float m_CurrentWaitTime;
	protected bool m_LoopEnabled;
	protected bool m_ReversePath;
	protected int m_Direction; // 1 = forward, -1 = reverse
	
	protected float m_MoveTimer;
	protected float m_StuckTimer;
	protected vector m_LastPosition;
	protected float m_StuckCheckInterval;
	
	void ABBotPatrol(ABBot bot)
	{
		m_Bot = bot;
		m_Waypoints = new array<vector>();
		m_WaitTimes = new array<float>();
		m_CurrentWaypointIndex = 0;
		m_IsWaiting = false;
		m_WaitTimer = 0;
		m_CurrentWaitTime = 5.0;
		m_LoopEnabled = true;
		m_ReversePath = false;
		m_Direction = 1;
		m_MoveTimer = 0;
		m_StuckTimer = 0;
		m_LastPosition = vector.Zero;
		m_StuckCheckInterval = 5.0;
	}
	
	void SetWaypoints(array<ref ABWaypoint> waypoints)
	{
		m_Waypoints.Clear();
		m_WaitTimes.Clear();
		
		if (!waypoints)
			return;
		
		foreach (ABWaypoint wp : waypoints)
		{
			vector pos = wp.GetPositionVector();
			if (pos != vector.Zero)
			{
				// Ajustar Y para o terreno
				pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
				m_Waypoints.Insert(pos);
				m_WaitTimes.Insert(wp.WaitTime);
			}
		}
		
		if (m_Waypoints.Count() > 0)
		{
			m_CurrentWaypointIndex = 0;
			ABLogger.LogPatrol(m_Bot.GetName(), 0, m_Waypoints[0]);
		}
	}
	
	bool HasWaypoints()
	{
		return m_Waypoints && m_Waypoints.Count() > 0;
	}
	
	int GetWaypointCount()
	{
		if (!m_Waypoints)
			return 0;
		return m_Waypoints.Count();
	}
	
	vector GetCurrentWaypoint()
	{
		if (!HasWaypoints())
			return vector.Zero;
		
		if (m_CurrentWaypointIndex < 0 || m_CurrentWaypointIndex >= m_Waypoints.Count())
			m_CurrentWaypointIndex = 0;
		
		return m_Waypoints[m_CurrentWaypointIndex];
	}
	
	void UpdatePatrol(float deltaTime)
	{
		if (!m_Bot || !m_Bot.IsAlive() || !HasWaypoints())
			return;
		
		if (m_IsWaiting)
		{
			UpdateWaiting(deltaTime);
			return;
		}
		
		UpdateMovement(deltaTime);
	}
	
	protected void UpdateWaiting(float deltaTime)
	{
		m_WaitTimer += deltaTime;
		
		if (m_WaitTimer >= m_CurrentWaitTime)
		{
			m_IsWaiting = false;
			m_WaitTimer = 0;
			
			// Avancar para proximo waypoint
			AdvanceWaypoint();
		}
	}
	
	protected void UpdateMovement(float deltaTime)
	{
		m_MoveTimer += deltaTime;
		
		vector targetWaypoint = GetCurrentWaypoint();
		if (targetWaypoint == vector.Zero)
			return;
		
		float distToWaypoint = m_Bot.DistanceTo(targetWaypoint);
		
		// Chegou ao waypoint
		if (distToWaypoint <= 3.0)
		{
			OnWaypointReached();
			return;
		}
		
		// Verificar se esta preso
		CheckStuck(deltaTime);
		
		// Mover em direcao ao waypoint
		float speed = 1.0;
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		if (diff)
			speed = diff.MovementSpeedMultiplier;
		
		m_Bot.MoveTo(targetWaypoint, speed);
		
		// Log de patrulha periodico
		if (m_MoveTimer >= 15.0)
		{
			m_MoveTimer = 0;
			if (ABConfig.s_Settings && ABConfig.s_Settings.LogPatrol)
			{
				ABLogger.LogPatrol(m_Bot.GetName(), m_CurrentWaypointIndex, m_Bot.GetPosition());
			}
		}
	}
	
	protected void OnWaypointReached()
	{
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogPatrol)
		{
			ABLogger.LogPatrol(m_Bot.GetName(), m_CurrentWaypointIndex, GetCurrentWaypoint());
		}
		
		// Iniciar espera no waypoint
		m_IsWaiting = true;
		m_WaitTimer = 0;
		
		if (m_CurrentWaypointIndex < m_WaitTimes.Count())
			m_CurrentWaitTime = m_WaitTimes[m_CurrentWaypointIndex];
		else
			m_CurrentWaitTime = 5.0;
	}
	
	protected void AdvanceWaypoint()
	{
		if (!HasWaypoints())
			return;
		
		int count = m_Waypoints.Count();
		
		if (m_LoopEnabled)
		{
			// Ciclo loop - voltar ao inicio quando chegar ao fim
			m_CurrentWaypointIndex = (m_CurrentWaypointIndex + m_Direction) % count;
			
			if (m_CurrentWaypointIndex < 0)
				m_CurrentWaypointIndex = count - 1;
		}
		else if (m_ReversePath)
		{
			// Ping-pong - ida e volta
			m_CurrentWaypointIndex += m_Direction;
			
			if (m_CurrentWaypointIndex >= count)
			{
				m_Direction = -1;
				m_CurrentWaypointIndex = count - 2;
				if (m_CurrentWaypointIndex < 0)
					m_CurrentWaypointIndex = 0;
			}
			else if (m_CurrentWaypointIndex < 0)
			{
				m_Direction = 1;
				m_CurrentWaypointIndex = 1;
				if (m_CurrentWaypointIndex >= count)
					m_CurrentWaypointIndex = 0;
			}
		}
		else
		{
			// Linear - parar no ultimo
			m_CurrentWaypointIndex += m_Direction;
			if (m_CurrentWaypointIndex >= count)
				m_CurrentWaypointIndex = count - 1;
		}
		
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogPatrol)
		{
			ABLogger.LogPatrol(m_Bot.GetName(), m_CurrentWaypointIndex, GetCurrentWaypoint());
		}
	}
	
	protected void CheckStuck(float deltaTime)
	{
		m_StuckTimer += deltaTime;
		
		if (m_StuckTimer >= m_StuckCheckInterval)
		{
			m_StuckTimer = 0;
			
			vector currentPos = m_Bot.GetPosition();
			
			if (m_LastPosition != vector.Zero)
			{
				float movedDist = vector.Distance(m_LastPosition, currentPos);
				
				// Se nao se moveu quase nada em 5 segundos, pode estar preso
				if (movedDist < 1.0)
				{
					ABLogger.Log("WARN", "PATROL", "Bot '" + m_Bot.GetName() + "' pode estar preso! Tentando contornar...");
					
					// Tentar contornar obstaculo
					vector waypointDir = GetCurrentWaypoint() - currentPos;
					waypointDir[1] = 0;
					waypointDir.Normalize();
					
					// Mover lateralmente para tentar contornar
					vector sideStep = Vector(-waypointDir[2], 0, waypointDir[0]) * 5.0;
					m_Bot.MoveTo(currentPos + sideStep, 1.0);
				}
			}
			
			m_LastPosition = currentPos;
		}
	}
	
	void SetLoopEnabled(bool enabled)
	{
		m_LoopEnabled = enabled;
	}
	
	void SetReversePath(bool enabled)
	{
		m_ReversePath = enabled;
	}
	
	int GetCurrentWaypointIndex()
	{
		return m_CurrentWaypointIndex;
	}
	
	bool IsWaiting()
	{
		return m_IsWaiting;
	}
};
