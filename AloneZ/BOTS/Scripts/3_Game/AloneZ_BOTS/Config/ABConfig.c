// AloneZ BOTS - Main Configuration Loader
class ABConfig
{
	static const string CONFIG_PATH = "$profile:AloneZ/BOTS/";
	static const string SETTINGS_FILE = "BotSettings.json";
	static const string DIFFICULTY_FOLDER = "Difficulty/";
	static const string SPAWNS_FOLDER = "Spawns/";
	static const string LOOT_FOLDER = "Loot/";
	
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
		
		EnsureDirectories();
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
	
	private static void EnsureDirectories()
	{
		if (!FileExist(CONFIG_PATH))
			MakeDirectory(CONFIG_PATH);
		if (!FileExist(CONFIG_PATH + DIFFICULTY_FOLDER))
			MakeDirectory(CONFIG_PATH + DIFFICULTY_FOLDER);
		if (!FileExist(CONFIG_PATH + SPAWNS_FOLDER))
			MakeDirectory(CONFIG_PATH + SPAWNS_FOLDER);
		if (!FileExist(CONFIG_PATH + LOOT_FOLDER))
			MakeDirectory(CONFIG_PATH + LOOT_FOLDER);
		if (!FileExist(CONFIG_PATH + "Logs/"))
			MakeDirectory(CONFIG_PATH + "Logs/");
	}
	
	private static void LoadSettings()
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
	
	private static void LoadDifficulties()
	{
		s_Difficulties = new map<string, ref ABDifficultyConfig>();
		
		ref array<string> diffFiles = {"Easy", "Medium", "Hard"};
		
		foreach (string diffName : diffFiles)
		{
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
	
	private static void LoadSpawnPoints()
	{
		s_SpawnPoints = new array<ref ABSpawnPointConfig>();
		string path = CONFIG_PATH + SPAWNS_FOLDER + "SpawnPoints.json";
		
		if (FileExist(path))
		{
			ref ABSpawnPointList spawnList = new ABSpawnPointList();
			JsonFileLoader<ABSpawnPointList>.JsonLoadFile(path, spawnList);
			
			if (spawnList.SpawnPoints)
			{
				foreach (ref ABSpawnPointConfig sp : spawnList.SpawnPoints)
				{
					s_SpawnPoints.Insert(sp);
				}
			}
			
			ABLogger.Log("INFO", "CONFIG", "SpawnPoints.json carregado com " + s_SpawnPoints.Count().ToString() + " pontos de spawn");
		}
		else
		{
			ref ABSpawnPointList defaultList = new ABSpawnPointList();
			defaultList.SetDefaults();
			JsonFileLoader<ABSpawnPointList>.JsonSaveFile(path, defaultList);
			
			foreach (ref ABSpawnPointConfig defSp : defaultList.SpawnPoints)
			{
				s_SpawnPoints.Insert(defSp);
			}
			
			ABLogger.Log("INFO", "CONFIG", "SpawnPoints.json criado com exemplos padrao");
		}
	}
	
	private static void LoadLootTables()
	{
		s_LootTables = new map<string, ref ABLootTable>();
		
		ref array<string> lootFiles = {"LootEasy", "LootMedium", "LootHard"};
		ref array<string> diffNames = {"Easy", "Medium", "Hard"};
		
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
