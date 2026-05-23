class ALZAgentReport
{
	static void Send(PlayerBase player)
	{
		if (!player) return;

		float _bodyTemp;
		if (player && player.IsSyncedModifierActive(eModifierSyncIDs.MODIFIER_SYNC_FEVER))
		{
			_bodyTemp = Math.RandomFloatInclusive(PlayerConstants.HIGH_TEMPERATURE_L, PlayerConstants.HIGH_TEMPERATURE_H);
		}
		else
		{
			_bodyTemp = Math.RandomFloatInclusive(PlayerConstants.NORMAL_TEMPERATURE_L, PlayerConstants.NORMAL_TEMPERATURE_H);
		}
		_bodyTemp = Math.Round(_bodyTemp * 10) / 10.0;

		float _airTemp = 0.0;
		Mission _m = GetGame().GetMission();
		if (_m)
		{
			WorldData _wd = _m.GetWorldData();
			if (_wd)
			{
				_airTemp = _wd.GetBaseEnvTemperatureAtObject(player);
			}
		}
		float _airTempR  = Math.Round(_airTemp  * 10) / 10.0;
		player.MessageStatus(string.Format("Temperatura corporal: %1°C", _bodyTemp));
		player.MessageStatus(string.Format("Temperatura do ar: %1°C", _airTempR));

		array<int> agentList = new array<int>();
		agentList.Insert(eAgents.BRAIN);
		agentList.Insert(eAgents.CHOLERA);
		agentList.Insert(eAgents.SALMONELLA);
		agentList.Insert(eAgents.INFLUENZA);
		agentList.Insert(eAgents.FOOD_POISON);
		agentList.Insert(eAgents.CHEMICAL_POISON);
		agentList.Insert(eAgents.WOUND_AGENT);
		agentList.Insert(eAgents.HEAVYMETAL);

		array<string> names = {"Kuru", "Cólera", "Salmonella", "Gripe", "Intoxicação alimentar", "Intoxicação química", "Infecção de ferida", "Metais pesados"};

		array<string> lines = new array<string>();
		for (int i = 0; i < agentList.Count(); i++)
		{
			int agentId = agentList[i];
			int c = player.GetSingleAgentCount(agentId);
			if (c > 0)
			{
				lines.Insert(string.Format("%1: %2", names[i], c));
			}
		}

		if (lines.Count() == 0)
		{
			player.MessageStatus("Болячки отсутствуют.");
			return;
		}

		player.MessageStatus("Болячки:");
		for (int k = 0; k < lines.Count(); k++)
			player.MessageStatus(lines[k]);
	}
}
