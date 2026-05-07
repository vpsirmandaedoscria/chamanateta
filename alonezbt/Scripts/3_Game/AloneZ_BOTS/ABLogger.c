class ABLogger
{
	static string LOG_FOLDER = "$profile:AloneZ\\BOTS\\Logs\\";
	static string LOG_FILE_PREFIX = "ABBot_";
	static bool s_Initialized;
	
	static void Init()
	{
		if (s_Initialized)
			return;
		
		CreateProfileDirs();
		
		s_Initialized = true;
		Log("INFO", "SYSTEM", "AloneZ BOTS Logger inicializado");
	}
	
	static void CreateProfileDirs()
	{
		string baseDir = "$profile:AloneZ";
		string botsDir = "$profile:AloneZ\\BOTS";
		string logsDir = "$profile:AloneZ\\BOTS\\Logs";
		string diffDir = "$profile:AloneZ\\BOTS\\Difficulty";
		string spawnDir = "$profile:AloneZ\\BOTS\\Spawns";
		string lootDir = "$profile:AloneZ\\BOTS\\Loot";
		
		if (!FileExist(baseDir))
			MakeDirectory(baseDir);
		if (!FileExist(botsDir))
			MakeDirectory(botsDir);
		if (!FileExist(logsDir))
			MakeDirectory(logsDir);
		if (!FileExist(diffDir))
			MakeDirectory(diffDir);
		if (!FileExist(spawnDir))
			MakeDirectory(spawnDir);
		if (!FileExist(lootDir))
			MakeDirectory(lootDir);
	}
	
	static void Close()
	{
		s_Initialized = false;
	}
	
	static string GetLogFilePath()
	{
		int year, month, day;
		GetYearMonthDay(year, month, day);
		string dateStr = year.ToString() + "-" + FormatNum(month) + "-" + FormatNum(day);
		return LOG_FOLDER + LOG_FILE_PREFIX + dateStr + ".log";
	}
	
	static string FormatNum(int num)
	{
		if (num < 10)
			return "0" + num.ToString();
		return num.ToString();
	}
	
	static string GetTimestamp()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		string ts = "[" + year.ToString() + "-" + FormatNum(month) + "-" + FormatNum(day) + " " + FormatNum(hour) + ":" + FormatNum(minute) + ":" + FormatNum(second) + "]";
		return ts;
	}
	
	static void Log(string level, string category, string message)
	{
		if (!s_Initialized)
			Init();
		
		string logLine = GetTimestamp() + " [" + level + "] [" + category + "] " + message;
		
		string filePath = GetLogFilePath();
		FileHandle file = OpenFile(filePath, FileMode.APPEND);
		if (file != 0)
		{
			FPrintln(file, logLine);
			CloseFile(file);
		}
		
		Print("[AloneZ_BOTS] " + logLine);
	}
	
	static void LogSpawn(string botName, string difficulty, vector position, string groupName)
	{
		string posStr = "X:" + position[0].ToString() + " Y:" + position[1].ToString() + " Z:" + position[2].ToString();
		Log("INFO", "SPAWN", "Bot '" + botName + "' spawned | Dificuldade: " + difficulty + " | Posicao: " + posStr + " | Grupo: " + groupName);
	}
	
	static void LogInit(string botName, string loadout, string difficulty)
	{
		Log("INFO", "INIT", "Bot '" + botName + "' inicializado | Loadout: " + loadout + " | Dificuldade: " + difficulty);
	}
	
	static void LogPosition(string botName, vector position)
	{
		string posStr = "X:" + position[0].ToString() + " Y:" + position[1].ToString() + " Z:" + position[2].ToString();
		Log("DEBUG", "POSITION", "Bot '" + botName + "' | Posicao: " + posStr);
	}
	
	static void LogDetection(string botName, string playerName, float distance)
	{
		Log("INFO", "DETECTION", "Bot '" + botName + "' detectou player '" + playerName + "' a " + distance.ToString() + "m");
	}
	
	static void LogStealth(string botName, string playerName)
	{
		Log("INFO", "STEALTH", "Bot '" + botName + "' em modo furtivo seguindo '" + playerName + "'");
	}
	
	static void LogCombatStart(string botName, string playerName, string combatType)
	{
		Log("INFO", "COMBAT", "Bot '" + botName + "' iniciou combate com '" + playerName + "' | Tipo: " + combatType);
	}
	
	static void LogCombatShot(string botName, string playerName, bool hit, float damage)
	{
		string hitStr = "MISS";
		if (hit)
			hitStr = "HIT (Dano: " + damage.ToString() + ")";
		Log("INFO", "COMBAT", "Bot '" + botName + "' disparou contra '" + playerName + "' | " + hitStr);
	}
	
	static void LogCombatMelee(string botName, string playerName, bool hit, float damage)
	{
		string hitStr = "MISS";
		if (hit)
			hitStr = "HIT (Dano: " + damage.ToString() + ")";
		Log("INFO", "MELEE", "Bot '" + botName + "' atacou corpo-a-corpo '" + playerName + "' | " + hitStr);
	}
	
	static void LogPatrol(string botName, int waypointIndex, vector position)
	{
		string posStr = "X:" + position[0].ToString() + " Y:" + position[1].ToString() + " Z:" + position[2].ToString();
		Log("DEBUG", "PATROL", "Bot '" + botName + "' movendo para waypoint #" + waypointIndex.ToString() + " | Posicao: " + posStr);
	}
	
	static void LogDeath(string botName, string killerName, vector position)
	{
		string posStr = "X:" + position[0].ToString() + " Y:" + position[1].ToString() + " Z:" + position[2].ToString();
		Log("INFO", "DEATH", "Bot '" + botName + "' morreu | Morto por: " + killerName + " | Posicao: " + posStr);
	}
	
	static void LogLoot(string botName, string itemClass, int quantity)
	{
		Log("INFO", "LOOT", "Bot '" + botName + "' drop: " + itemClass + " x" + quantity.ToString());
	}
	
	static void LogDespawn(string botName, string reason)
	{
		Log("INFO", "DESPAWN", "Bot '" + botName + "' despawnado | Razao: " + reason);
	}
	
	static void LogGroupCreated(string groupName, int size, string difficulty)
	{
		Log("INFO", "GROUP", "Grupo '" + groupName + "' criado | Tamanho: " + size.ToString() + " | Dificuldade: " + difficulty);
	}
	
	static void LogRespawn(string groupName, vector position)
	{
		string posStr = "X:" + position[0].ToString() + " Y:" + position[1].ToString() + " Z:" + position[2].ToString();
		Log("INFO", "RESPAWN", "Grupo '" + groupName + "' respawnando em " + posStr);
	}
	
	static void LogError(string category, string message)
	{
		Log("ERROR", category, message);
	}
	
	static void LogStateChange(string botName, string fromState, string toState)
	{
		Log("DEBUG", "STATE", "Bot '" + botName + "' mudou de " + fromState + " para " + toState);
	}
};
