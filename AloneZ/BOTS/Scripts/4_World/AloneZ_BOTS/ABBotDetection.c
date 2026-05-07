// AloneZ BOTS - Player Detection System
// Detecao de players num raio configuravel (padrao 100m)

class ABBotDetection
{
	protected ABBot m_Bot;
	protected float m_ScanTimer;
	protected float m_ScanInterval;
	protected PlayerBase m_LastDetectedPlayer;
	protected float m_LastDetectedDistance;
	
	void ABBotDetection(ABBot bot)
	{
		m_Bot = bot;
		m_ScanTimer = 0;
		m_ScanInterval = 1.0;
		m_LastDetectedPlayer = null;
		m_LastDetectedDistance = -1;
	}
	
	PlayerBase ScanForPlayers()
	{
		if (!m_Bot || !m_Bot.IsAlive())
			return null;
		
		float detectionRadius = 100.0;
		
		ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
		if (diff)
			detectionRadius = diff.DetectionRadius;
		else if (ABConfig.s_Settings)
			detectionRadius = ABConfig.s_Settings.DetectionRadius;
		
		vector botPos = m_Bot.GetPosition();
		
		PlayerBase closestPlayer = null;
		float closestDist = detectionRadius + 1.0;
		
		// Buscar todos os players no raio
		ref array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (!player || !player.IsAlive())
				continue;
			
			// Ignorar bots do mesmo sistema
			if (IsABBot(player))
				continue;
			
			float dist = vector.Distance(botPos, player.GetPosition());
			
			if (dist <= detectionRadius && dist < closestDist)
			{
				// Line of sight check
				if (HasLineOfSight(botPos, player.GetPosition()))
				{
					closestPlayer = player;
					closestDist = dist;
				}
			}
		}
		
		m_LastDetectedPlayer = closestPlayer;
		m_LastDetectedDistance = closestDist;
		
		return closestPlayer;
	}
	
	bool HasLineOfSight(vector from, vector to)
	{
		vector start = from + Vector(0, 1.6, 0); // Altura dos olhos
		vector end = to + Vector(0, 1.0, 0);     // Centro de massa do player
		
		vector hitPos;
		vector hitNormal;
		int contactComponent;
		
		set<Object> results = new set<Object>();
		
		if (DayZPhysics.RaycastRV(start, end, hitPos, hitNormal, contactComponent, results, null, m_Bot.GetEntity(), false, false, ObjIntersectGeom, 0.0, CollisionFlags.ALLOBJECTS))
		{
			// Se bateu em algo, verificar se eh o player
			if (results.Count() > 0)
			{
				foreach (Object obj : results)
				{
					PlayerBase hitPlayer = PlayerBase.Cast(obj);
					if (hitPlayer)
						return true;
					
					// Se bateu em parede/terreno antes do player
					if (obj.IsBuilding() || obj.IsRock() || obj.IsTree())
						return false;
				}
			}
			return false;
		}
		
		// Nenhuma obstrucao = tem visao
		return true;
	}
	
	bool IsABBot(PlayerBase player)
	{
		if (!player)
			return false;
		
		// Verificar se o player eh um bot gerenciado pelo ABBotManager
		string typeName = player.GetType();
		if (typeName.IndexOf("AB_Survivor") == 0)
			return true;
		
		return false;
	}
	
	PlayerBase GetLastDetectedPlayer()
	{
		return m_LastDetectedPlayer;
	}
	
	float GetLastDetectedDistance()
	{
		return m_LastDetectedDistance;
	}
	
	// Verifica se um player especifico ainda esta no raio
	bool IsPlayerInRange(PlayerBase player, float customRadius = -1)
	{
		if (!player || !player.IsAlive() || !m_Bot || !m_Bot.IsAlive())
			return false;
		
		float radius = customRadius;
		if (radius < 0)
		{
			ABDifficultyConfig diff = m_Bot.GetDifficultyConfig();
			if (diff)
				radius = diff.DetectionRadius;
			else
				radius = 100.0;
		}
		
		float dist = vector.Distance(m_Bot.GetPosition(), player.GetPosition());
		return dist <= radius;
	}
};
