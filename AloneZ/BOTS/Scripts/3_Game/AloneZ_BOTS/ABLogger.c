// AloneZ BOTS - Sistema de Logging Completo
class ABLogger
{
	static const string LOG_FOLDER = "$profile:AloneZ/BOTS/Logs/";
	static const string LOG_FILE_PREFIX = "ABBot_";
	
	private static FileHandle s_LogFile;
	private static string s_CurrentLogDate;
	private static bool s_Initialized;
	
	static void Init()
	{
		if (s_Initialized)
			return;
		
		if (!FileExist(LOG_FOLDER))
			MakeDirectory(LOG_FOLDER);
		
		s_Initialized = true;
		Log("INFO", "SYSTEM", "AloneZ BOTS Logger inicializado");
	}
	
	static void Close()
	{
		if (s_LogFile != 0)
		{
			CloseFile(s_LogFile);
			s_LogFile = 0;
		}
		s_Initialized = false;
	}
	
	private static void EnsureLogFile()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		
		string dateStr = year.ToString() + "-" + FormatNum(month) + "-" + FormatNum(day);
		
		if (dateStr != s_CurrentLogDate || s_LogFile == 0)
		{
			if (s_LogFile != 0)
				CloseFile(s_LogFile);
			
			s_CurrentLogDate = dateStr;
			string filePath = LOG_FOLDER + LOG_FILE_PREFIX + dateStr + ".log";
			s_LogFile = OpenFile(filePath, FileMode.APPEND);
		}
	}
	
	private static string FormatNum(int num)
	{
		if (num < 10)
			return "0" + num.ToString();
		return num.ToString();
	}
	
	private static string GetTimestamp()
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		
		return "[" + year.ToString() + "-" + FormatNum(month) + "-" + FormatNum(day) 
			+ " " + FormatNum(hour) + ":" + FormatNum(minute) + ":" + FormatNum(second) + "]";
	}
	
	static void Log(string level, string category, string message)
	{
		if (!s_Initialized)
			Init();
		
		EnsureLogFile();
		
		string logLine = GetTimestamp() + " [" + level + "] [" + category + "] " + message;
		
		if (s_LogFile != 0)
		{
			FPrintln(s_LogFile, logLine);
		}
		
		PrintToRPT("[AloneZ_BOTS] " + logLine);
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
		Log("INFO", "LOOT", "Bot '" + botName + "' dropou loot: " + itemClass + " x" + quantity.ToString());
	}
	
	static void LogDespawn(string botName, string reason)
	{
		Log("INFO", "DESPAWN", "Bot '" + botName + "' removido | Motivo: " + reason);
	}
	
	static void LogGroupCreated(string groupName, int botCount, string difficulty)
	{
		Log("INFO", "GROUP", "Grupo '" + groupName + "' criado com " + botCount.ToString() + " bots | Dificuldade: " + difficulty);
	}
	
	static void LogRespawn(string groupName, vector position)
	{
		string posStr = "X:" + position[0].ToString() + " Y:" + position[1].ToString() + " Z:" + position[2].ToString();
		Log("INFO", "RESPAWN", "Grupo '" + groupName + "' respawnando em " + posStr);
	}
	
	static void LogError(string context, string message)
	{
		Log("ERROR", context, message);
	}
	
	static void LogStateChange(string botName, string fromState, string toState)
	{
		Log("DEBUG", "STATE", "Bot '" + botName + "' mudou estado: " + fromState + " -> " + toState);
	}
};
