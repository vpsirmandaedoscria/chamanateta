// AloneZ BOTS - Spawn Manager
// Gerencia spawn e respawn de todos os bots

class ABSpawnManager
{
	static ref array<ref ABBotGroup> s_Groups;
	static ref array<ref ABBot> s_AllBots;
	static bool s_Initialized;
	static int s_BotCounter;
	
	static ref array<string> s_BotTypes = {
		"AB_SurvivorM_Mirek",
		"AB_SurvivorM_Denis",
		"AB_SurvivorM_Boris",
		"AB_SurvivorM_Cyril",
		"AB_SurvivorM_Elias",
		"AB_SurvivorM_Francis",
		"AB_SurvivorM_Guo",
		"AB_SurvivorM_Hassan",
		"AB_SurvivorM_Indar",
		"AB_SurvivorM_Jose",
		"AB_SurvivorM_Kaito",
		"AB_SurvivorM_Lewis",
		"AB_SurvivorM_Manua",
		"AB_SurvivorM_Niki",
		"AB_SurvivorM_Oliver",
		"AB_SurvivorM_Peter",
		"AB_SurvivorM_Quinn",
		"AB_SurvivorM_Rolf",
		"AB_SurvivorM_Seth",
		"AB_SurvivorM_Taiki",
		"AB_SurvivorF_Linda",
		"AB_SurvivorF_Maria",
		"AB_SurvivorF_Frida",
		"AB_SurvivorF_Gabi",
		"AB_SurvivorF_Helga",
		"AB_SurvivorF_Irena",
		"AB_SurvivorF_Judy",
		"AB_SurvivorF_Keiko",
		"AB_SurvivorF_Eva",
		"AB_SurvivorF_Naomi",
		"AB_SurvivorF_Baty"
	};
	
	static void Init()
	{
		if (s_Initialized)
			return;
		
		s_Groups = new array<ref ABBotGroup>();
		s_AllBots = new array<ref ABBot>();
		s_BotCounter = 0;
		s_Initialized = true;
		
		ABLogger.Log("INFO", "SPAWN_MGR", "ABSpawnManager inicializado");
	}
	
	static void SpawnAllConfigured()
	{
		if (!ABConfig.s_SpawnPoints)
		{
			ABLogger.LogError("SPAWN_MGR", "Nenhum ponto de spawn configurado!");
			return;
		}
		
		ABLogger.Log("INFO", "SPAWN_MGR", "Iniciando spawn de " + ABConfig.s_SpawnPoints.Count().ToString() + " pontos configurados...");
		
		foreach (ABSpawnPointConfig spawnConfig : ABConfig.s_SpawnPoints)
		{
			if (!spawnConfig.Enabled)
			{
				ABLogger.Log("INFO", "SPAWN_MGR", "Spawn '" + spawnConfig.Name + "' desabilitado, pulando...");
				continue;
			}
			
			// Verificar chance de spawn
			if (Math.RandomFloat01() > spawnConfig.SpawnChance)
			{
				ABLogger.Log("INFO", "SPAWN_MGR", "Spawn '" + spawnConfig.Name + "' nao passou no teste de chance (" + spawnConfig.SpawnChance.ToString() + ")");
				continue;
			}
			
			// Verificar limite total de bots
			if (ABConfig.s_Settings && s_AllBots.Count() >= ABConfig.s_Settings.MaxBotsTotal)
			{
				ABLogger.Log("WARN", "SPAWN_MGR", "Limite maximo de bots atingido (" + ABConfig.s_Settings.MaxBotsTotal.ToString() + ")");
				break;
			}
			
			SpawnGroup(spawnConfig);
		}
		
		ABLogger.Log("INFO", "SPAWN_MGR", "Spawn completo! Total de bots: " + s_AllBots.Count().ToString() + " | Total de grupos: " + s_Groups.Count().ToString());
	}
	
	static ABBotGroup SpawnGroup(ABSpawnPointConfig spawnConfig)
	{
		if (!spawnConfig)
			return null;
		
		vector spawnPos = spawnConfig.GetPositionVector();
		
		// Ajustar Y para o terreno
		spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
		
		ref ABBotGroup group = new ABBotGroup(
			spawnConfig.GroupName,
			spawnConfig.Difficulty,
			spawnPos,
			spawnConfig.GroupSize
		);
		
		group.SetSpawnConfig(spawnConfig);
		
		// Spawnar bots do grupo
		for (int i = 0; i < spawnConfig.GroupSize; i++)
		{
			// Posicao com offset para nao spawnar todos no mesmo ponto
			vector botPos = spawnPos + Vector(Math.RandomFloat(-3.0, 3.0), 0, Math.RandomFloat(-3.0, 3.0));
			botPos[1] = GetGame().SurfaceY(botPos[0], botPos[2]);
			
			ABBot bot = SpawnBot(botPos, spawnConfig.Difficulty, spawnConfig.GroupName, spawnConfig.Loadout);
			
			if (bot)
			{
				group.AddMember(bot);
				
				// Configurar waypoints de patrulha
				if (spawnConfig.Waypoints && spawnConfig.Waypoints.Count() > 0)
				{
					bot.GetPatrol().SetWaypoints(spawnConfig.Waypoints);
					bot.SetState(ABBotState.PATROLLING);
				}
			}
		}
		
		s_Groups.Insert(group);
		
		return group;
	}
	
	static ABBot SpawnBot(vector position, string difficulty, string groupName, string loadout)
	{
		s_BotCounter++;
		string botName = "ABBot_" + s_BotCounter.ToString();
		
		// Selecionar tipo aleatorio de survivor
		string botType = GetRandomBotType();
		
		ABLogger.Log("INFO", "SPAWN_MGR", "Spawning bot '" + botName + "' tipo '" + botType + "' em " + position.ToString());
		
		// Criar entidade
		PlayerBase botEntity = PlayerBase.Cast(GetGame().CreateObjectEx(botType, position, ECE_PLACE_ON_SURFACE));
		
		if (!botEntity)
		{
			ABLogger.LogError("SPAWN_MGR", "Falha ao criar entidade do bot '" + botName + "'!");
			return null;
		}
		
		// Criar wrapper do bot
		ref ABBot bot = new ABBot(botEntity, botName, difficulty, position, groupName);
		
		// Equipar o bot
		ABBotLoot.EquipBot(botEntity, difficulty, loadout);
		
		s_AllBots.Insert(bot);
		
		if (ABConfig.s_Settings && ABConfig.s_Settings.LogSpawns)
		{
			ABLogger.LogSpawn(botName, difficulty, position, groupName);
		}
		
		return bot;
	}
	
	static string GetRandomBotType()
	{
		if (!s_BotTypes || s_BotTypes.Count() == 0)
			return "AB_SurvivorM_Mirek";
		
		int idx = Math.RandomInt(0, s_BotTypes.Count());
		return s_BotTypes[idx];
	}
	
	static void Update(float deltaTime)
	{
		if (!s_Initialized)
			return;
		
		// Atualizar todos os grupos
		foreach (ABBotGroup group : s_Groups)
		{
			if (group)
				group.Update(deltaTime);
		}
	}
	
	static void RequestGroupRespawn(ABBotGroup group)
	{
		if (!group)
			return;
		
		ABSpawnPointConfig config = group.GetSpawnConfig();
		if (!config)
		{
			ABLogger.LogError("SPAWN_MGR", "Configuracao de spawn nao encontrada para grupo '" + group.GetGroupName() + "'");
			return;
		}
		
		ABLogger.Log("INFO", "SPAWN_MGR", "Respawnando grupo '" + group.GetGroupName() + "'...");
		
		// Limpar bots mortos
		group.CleanupDeadBots();
		
		// Respawnar bots
		vector spawnPos = group.GetSpawnPosition();
		
		for (int i = group.GetAliveCount(); i < group.GetMaxSize(); i++)
		{
			vector botPos = spawnPos + Vector(Math.RandomFloat(-3.0, 3.0), 0, Math.RandomFloat(-3.0, 3.0));
			botPos[1] = GetGame().SurfaceY(botPos[0], botPos[2]);
			
			ABBot bot = SpawnBot(botPos, config.Difficulty, config.GroupName, config.Loadout);
			if (bot)
			{
				group.AddMember(bot);
				
				if (config.Waypoints && config.Waypoints.Count() > 0)
				{
					bot.GetPatrol().SetWaypoints(config.Waypoints);
					bot.SetState(ABBotState.PATROLLING);
				}
			}
		}
	}
	
	static void Cleanup()
	{
		ABLogger.Log("INFO", "SPAWN_MGR", "Limpando todos os bots...");
		
		if (s_AllBots)
		{
			foreach (ABBot bot : s_AllBots)
			{
				if (bot)
				{
					PlayerBase entity = bot.GetEntity();
					if (entity)
					{
						GetGame().ObjectDelete(entity);
					}
				}
			}
			s_AllBots.Clear();
		}
		
		if (s_Groups)
			s_Groups.Clear();
		
		s_Initialized = false;
	}
	
	static int GetTotalBotCount()
	{
		if (!s_AllBots)
			return 0;
		return s_AllBots.Count();
	}
	
	static int GetAliveBotCount()
	{
		int count = 0;
		if (s_AllBots)
		{
			foreach (ABBot bot : s_AllBots)
			{
				if (bot && bot.IsAlive())
					count++;
			}
		}
		return count;
	}
	
	static int GetGroupCount()
	{
		if (!s_Groups)
			return 0;
		return s_Groups.Count();
	}
};
