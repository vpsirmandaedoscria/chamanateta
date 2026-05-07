class ABConfig
{
	static string CONFIG_PATH = "$profile:AloneZ\\BOTS\\";
	static string SETTINGS_FILE = "BotSettings.json";
	static string DIFFICULTY_FOLDER = "Difficulty\\";
	static string SPAWNS_FOLDER = "Spawns\\";
	static string LOOT_FOLDER = "Loot\\";
	
	static ref ABBotSettings s_Settings;
	static ref map<string, ref ABDifficultyConfig> s_Difficulties;
	static ref array<ref ABSpawnPointConfig> s_SpawnPoints;
	static ref map<string, ref ABLootTable> s_LootTables;
	
	static bool s_Loaded;
	
	static void Load()
	{
		if (s_Loaded)
			return;
		
		ABLogger.Init();
		ABLogger.Log("INFO", "CONFIG", "Carregando configuracoes AloneZ BOTS...");
		
		LoadSettings();
		LoadDifficulties();
		LoadSpawnPoints();
		LoadLootTables();
		
		s_Loaded = true;
		ABLogger.Log("INFO", "CONFIG", "Configuracoes carregadas com sucesso!");
	}
	
	static void Reload()
	{
		s_Loaded = false;
		Load();
	}
	
	static void LoadSettings()
	{
		s_Settings = new ABBotSettings();
		string path = CONFIG_PATH + SETTINGS_FILE;
		
		if (FileExist(path))
		{
			JsonFileLoader<ABBotSettings>.JsonLoadFile(path, s_Settings);
			ABLogger.Log("INFO", "CONFIG", "BotSettings.json carregado");
		}
		else
		{
			s_Settings.SetDefaults();
			JsonFileLoader<ABBotSettings>.JsonSaveFile(path, s_Settings);
			ABLogger.Log("INFO", "CONFIG", "BotSettings.json criado com valores padrao");
		}
	}
	
	static void LoadDifficulties()
	{
		s_Difficulties = new map<string, ref ABDifficultyConfig>();
		
		ref array<string> diffFiles = new array<string>();
		diffFiles.Insert("Easy");
		diffFiles.Insert("Medium");
		diffFiles.Insert("Hard");
		
		for (int i = 0; i < diffFiles.Count(); i++)
		{
			string diffName = diffFiles[i];
			string path = CONFIG_PATH + DIFFICULTY_FOLDER + diffName + ".json";
			ref ABDifficultyConfig diff = new ABDifficultyConfig();
			
			if (FileExist(path))
			{
				JsonFileLoader<ABDifficultyConfig>.JsonLoadFile(path, diff);
				ABLogger.Log("INFO", "CONFIG", "Dificuldade '" + diffName + "' carregada");
			}
			else
			{
				diff.SetDefaults(diffName);
				JsonFileLoader<ABDifficultyConfig>.JsonSaveFile(path, diff);
				ABLogger.Log("INFO", "CONFIG", "Dificuldade '" + diffName + "' criada com valores padrao");
			}
			
			s_Difficulties.Set(diffName, diff);
		}
	}
	
	static void LoadSpawnPoints()
	{
		s_SpawnPoints = new array<ref ABSpawnPointConfig>();
		string path = CONFIG_PATH + SPAWNS_FOLDER + "SpawnPoints.json";
		
		if (FileExist(path))
		{
			ref ABSpawnPointList spawnList = new ABSpawnPointList();
			JsonFileLoader<ABSpawnPointList>.JsonLoadFile(path, spawnList);
			
			if (spawnList.SpawnPoints)
			{
				for (int i = 0; i < spawnList.SpawnPoints.Count(); i++)
				{
					s_SpawnPoints.Insert(spawnList.SpawnPoints[i]);
				}
			}
			
			ABLogger.Log("INFO", "CONFIG", "SpawnPoints.json carregado com " + s_SpawnPoints.Count().ToString() + " pontos de spawn");
		}
		else
		{
			ref ABSpawnPointList defaultList = new ABSpawnPointList();
			defaultList.SetDefaults();
			JsonFileLoader<ABSpawnPointList>.JsonSaveFile(path, defaultList);
			
			for (int j = 0; j < defaultList.SpawnPoints.Count(); j++)
			{
				s_SpawnPoints.Insert(defaultList.SpawnPoints[j]);
			}
			
			ABLogger.Log("INFO", "CONFIG", "SpawnPoints.json criado com exemplos padrao");
		}
	}
	
	static void LoadLootTables()
	{
		s_LootTables = new map<string, ref ABLootTable>();
		
		ref array<string> lootFiles = new array<string>();
		lootFiles.Insert("LootEasy");
		lootFiles.Insert("LootMedium");
		lootFiles.Insert("LootHard");
		
		ref array<string> diffNames = new array<string>();
		diffNames.Insert("Easy");
		diffNames.Insert("Medium");
		diffNames.Insert("Hard");
		
		for (int i = 0; i < lootFiles.Count(); i++)
		{
			string path = CONFIG_PATH + LOOT_FOLDER + lootFiles[i] + ".json";
			ref ABLootTable loot = new ABLootTable();
			
			if (FileExist(path))
			{
				JsonFileLoader<ABLootTable>.JsonLoadFile(path, loot);
				ABLogger.Log("INFO", "CONFIG", "Tabela de loot '" + lootFiles[i] + "' carregada");
			}
			else
			{
				loot.SetDefaults(diffNames[i]);
				JsonFileLoader<ABLootTable>.JsonSaveFile(path, loot);
				ABLogger.Log("INFO", "CONFIG", "Tabela de loot '" + lootFiles[i] + "' criada com valores padrao");
			}
			
			s_LootTables.Set(diffNames[i], loot);
		}
	}
	
	static ABDifficultyConfig GetDifficulty(string name)
	{
		if (s_Difficulties && s_Difficulties.Contains(name))
			return s_Difficulties.Get(name);
		
		if (s_Difficulties && s_Difficulties.Contains("Medium"))
			return s_Difficulties.Get("Medium");
		
		return null;
	}
	
	static ABLootTable GetLootTable(string difficulty)
	{
		if (s_LootTables && s_LootTables.Contains(difficulty))
			return s_LootTables.Get(difficulty);
		
		return null;
	}
};

class ABBotSettings
{
	bool Enabled;
	float BotUpdateInterval;
	float PositionLogInterval;
	float DetectionRadius;
	float StealthApproachDistance;
	float CombatEngageDistance;
	float MeleeEngageDistance;
	float DespawnDistance;
	float RespawnCooldown;
	int MaxBotsTotal;
	bool LogPositions;
	bool LogCombat;
	bool LogDetection;
	bool LogPatrol;
	bool LogSpawns;
	
	void SetDefaults()
	{
		Enabled = true;
		BotUpdateInterval = 1.0;
		PositionLogInterval = 30.0;
		DetectionRadius = 100.0;
		StealthApproachDistance = 50.0;
		CombatEngageDistance = 30.0;
		MeleeEngageDistance = 3.0;
		DespawnDistance = 500.0;
		RespawnCooldown = 300.0;
		MaxBotsTotal = 50;
		LogPositions = true;
		LogCombat = true;
		LogDetection = true;
		LogPatrol = true;
		LogSpawns = true;
	}
};
