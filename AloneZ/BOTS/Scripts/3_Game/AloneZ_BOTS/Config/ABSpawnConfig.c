// AloneZ BOTS - Spawn Point Configuration

class ABSpawnPointList
{
	ref array<ref ABSpawnPointConfig> SpawnPoints;
	
	void ABSpawnPointList()
	{
		SpawnPoints = new array<ref ABSpawnPointConfig>();
	}
	
	void SetDefaults()
	{
		// Exemplo de spawn 1 - Grupo de bots em Cherno
		ref ABSpawnPointConfig sp1 = new ABSpawnPointConfig();
		sp1.Name = "Cherno_Patrol_01";
		sp1.Enabled = true;
		sp1.Position = {6824.0, 0.0, 2480.0};
		sp1.Difficulty = "Medium";
		sp1.GroupSize = 3;
		sp1.GroupName = "Cherno_Squad_Alpha";
		sp1.RespawnEnabled = true;
		sp1.RespawnTime = 300.0;
		sp1.SpawnChance = 1.0;
		sp1.Loadout = "MilitaryLoadout";
		sp1.Waypoints = new array<ref ABWaypoint>();
		
		ref ABWaypoint wp1 = new ABWaypoint();
		wp1.Position = {6824.0, 0.0, 2480.0};
		wp1.WaitTime = 5.0;
		sp1.Waypoints.Insert(wp1);
		
		ref ABWaypoint wp2 = new ABWaypoint();
		wp2.Position = {6850.0, 0.0, 2510.0};
		wp2.WaitTime = 5.0;
		sp1.Waypoints.Insert(wp2);
		
		ref ABWaypoint wp3 = new ABWaypoint();
		wp3.Position = {6880.0, 0.0, 2470.0};
		wp3.WaitTime = 5.0;
		sp1.Waypoints.Insert(wp3);
		
		SpawnPoints.Insert(sp1);
		
		// Exemplo de spawn 2 - Grupo de bots em Elektro
		ref ABSpawnPointConfig sp2 = new ABSpawnPointConfig();
		sp2.Name = "Elektro_Guard_01";
		sp2.Enabled = true;
		sp2.Position = {10384.0, 0.0, 2254.0};
		sp2.Difficulty = "Hard";
		sp2.GroupSize = 2;
		sp2.GroupName = "Elektro_Guard_Team";
		sp2.RespawnEnabled = true;
		sp2.RespawnTime = 600.0;
		sp2.SpawnChance = 0.8;
		sp2.Loadout = "HeavyMilitaryLoadout";
		sp2.Waypoints = new array<ref ABWaypoint>();
		
		ref ABWaypoint wp4 = new ABWaypoint();
		wp4.Position = {10384.0, 0.0, 2254.0};
		wp4.WaitTime = 10.0;
		sp2.Waypoints.Insert(wp4);
		
		ref ABWaypoint wp5 = new ABWaypoint();
		wp5.Position = {10420.0, 0.0, 2290.0};
		wp5.WaitTime = 10.0;
		sp2.Waypoints.Insert(wp5);
		
		SpawnPoints.Insert(sp2);
		
		// Exemplo de spawn 3 - Bots faceis
		ref ABSpawnPointConfig sp3 = new ABSpawnPointConfig();
		sp3.Name = "Coast_Bandits_01";
		sp3.Enabled = true;
		sp3.Position = {8350.0, 0.0, 2500.0};
		sp3.Difficulty = "Easy";
		sp3.GroupSize = 4;
		sp3.GroupName = "Coast_Bandits";
		sp3.RespawnEnabled = true;
		sp3.RespawnTime = 900.0;
		sp3.SpawnChance = 0.6;
		sp3.Loadout = "CivilianLoadout";
		sp3.Waypoints = new array<ref ABWaypoint>();
		
		ref ABWaypoint wp6 = new ABWaypoint();
		wp6.Position = {8350.0, 0.0, 2500.0};
		wp6.WaitTime = 8.0;
		sp3.Waypoints.Insert(wp6);
		
		ref ABWaypoint wp7 = new ABWaypoint();
		wp7.Position = {8400.0, 0.0, 2550.0};
		wp7.WaitTime = 8.0;
		sp3.Waypoints.Insert(wp7);
		
		ref ABWaypoint wp8 = new ABWaypoint();
		wp8.Position = {8370.0, 0.0, 2580.0};
		wp8.WaitTime = 8.0;
		sp3.Waypoints.Insert(wp8);
		
		SpawnPoints.Insert(sp3);
	}
};

class ABSpawnPointConfig
{
	string Name;
	bool Enabled;
	ref array<float> Position;
	string Difficulty;          // "Easy", "Medium", "Hard"
	int GroupSize;
	string GroupName;
	bool RespawnEnabled;
	float RespawnTime;
	float SpawnChance;          // 0.0-1.0
	string Loadout;
	ref array<ref ABWaypoint> Waypoints;
	
	void ABSpawnPointConfig()
	{
		Position = new array<float>();
		Waypoints = new array<ref ABWaypoint>();
	}
	
	vector GetPositionVector()
	{
		if (Position && Position.Count() >= 3)
			return Vector(Position[0], Position[1], Position[2]);
		return vector.Zero;
	}
};

class ABWaypoint
{
	ref array<float> Position;
	float WaitTime;
	
	void ABWaypoint()
	{
		Position = new array<float>();
		WaitTime = 5.0;
	}
	
	vector GetPositionVector()
	{
		if (Position && Position.Count() >= 3)
			return Vector(Position[0], Position[1], Position[2]);
		return vector.Zero;
	}
};
