class ALZAgentReportSrv
{
	protected static void AppendLine(out string dst, string line)
	{
		if (dst == "") { dst = line; }
		else { dst = dst + "\n" + line; }
	}

	static void SendTo(PlayerBase player)
	{
		if (!player) { return; }

		string report = "";

		float bodyTemp;
		if (player.IsSyncedModifierActive(eModifierSyncIDs.MODIFIER_SYNC_FEVER))
		{
			bodyTemp = Math.RandomFloatInclusive(PlayerConstants.HIGH_TEMPERATURE_L, PlayerConstants.HIGH_TEMPERATURE_H);
		}
		else
		{
			bodyTemp = Math.RandomFloatInclusive(PlayerConstants.NORMAL_TEMPERATURE_L, PlayerConstants.NORMAL_TEMPERATURE_H);
		}
		bodyTemp = Math.Round(bodyTemp * 10) / 10.0;

		float airTemp = 0.0;
		Mission m = GetGame().GetMission();
		if (m)
		{
			WorldData wd = m.GetWorldData();
			if (wd)
			{
				airTemp = wd.GetBaseEnvTemperatureAtObject(player);
			}
		}
		float airTempR = Math.Round(airTemp * 10) / 10.0;

		AppendLine(report, string.Format("Temperatura corporal: %1°C", bodyTemp));
		AppendLine(report, string.Format("Temperatura do ar: %1°C", airTempR));
        int sZ=0,sA=0,sT=0,sD=0,sDe=0;
        if (ALZ_ReadProgressStages(player, sZ, sA, sT, sD, sDe))
        {
            AppendLine(report, "Levels de progresso:");
            AppendLine(report, "Zumbis: Level " + (sZ + 1).ToString());
            AppendLine(report, "Animais: Level " + (sA + 1).ToString());
            AppendLine(report, "Tempo: Level " + (sT + 1).ToString());
            AppendLine(report, "Distância: Level " + (sD + 1).ToString());
            AppendLine(report, "Mortes: Level " + (sDe + 1).ToString());
        }


		array<int> agents = new array<int>();
		agents.Insert(eAgents.BRAIN);
		agents.Insert(eAgents.CHOLERA);
		agents.Insert(eAgents.SALMONELLA);
		agents.Insert(eAgents.INFLUENZA);
		agents.Insert(eAgents.FOOD_POISON);
		agents.Insert(eAgents.WOUND_AGENT);
		agents.Insert(eAgents.HEAVYMETAL);

		array<string> names = new array<string>();
		names.Insert("Kuru");
		names.Insert("Cólera");
		names.Insert("Salmonella");
		names.Insert("Gripe");
		names.Insert("Intoxicação alimentar");
		names.Insert("Infecção de ferida");
		names.Insert("Metais pesados");

		int positive = 0;
		for (int i = 0; i < agents.Count(); i++)
		{
			int c = player.GetSingleAgentCount(agents[i]);
			if (c > 0)
			{
				positive++;
				AppendLine(report, string.Format("%1: %2", names[i], c));
			}
		}

		if (positive == 0) { AppendLine(report, "Doenças ausentes."); }
		else { report = "Você está doente:\n" + report; }

		Param1<string> p = new Param1<string>(report);
		GetGame().RPCSingleParam(player, /*rpc id*/ 80001, p, true, player.GetIdentity());
	}
}

class AloneZPlayerData_Lite
{
    int ZedStage;
    int AnimalStage;
    int TimeStage;
    int DistanceStage;
    int DeathStage;
    void AloneZPlayerData_Lite() { ZedStage = 0; AnimalStage = 0; TimeStage = 0; DistanceStage = 0; DeathStage = 0; }
}

static bool ALZ_ReadProgressStages(PlayerBase pb, out int zed, out int animal, out int timeS, out int dist, out int death)
{
    zed = 0; animal = 0; timeS = 0; dist = 0; death = 0;
    if (!pb || !pb.GetIdentity()) return false;
    string uid = pb.GetIdentity().GetPlainId();
    string path = "\$profile:AloneZ/Progress/players/" + uid + ".json";
    AloneZPlayerData_Lite data;
    if (FileExist(path)) JsonFileLoader<AloneZPlayerData_Lite>.JsonLoadFile(path, data);
    if (!data) { return true; }
    zed   = data.ZedStage;
    animal= data.AnimalStage;
    timeS = data.TimeStage;
    dist  = data.DistanceStage;
    death = data.DeathStage;
    return true;
}

