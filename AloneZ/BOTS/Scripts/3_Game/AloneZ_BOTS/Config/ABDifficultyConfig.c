class ABDifficultyConfig
{
	string Name;
	float AccuracyMin;
	float AccuracyMax;
	float DamageMultiplier;
	float DamageReceivedMultiplier;
	float ReactionTime;
	float DetectionRadius;
	float MovementSpeedMultiplier;
	float Brutality;
	float MeleeChance;
	float MeleeDamage;
	float FireRate;
	float AimSpread;
	float HealthMultiplier;
	float StealthDistance;
	float StealthSpeedMultiplier;
	float DodgeChance;
	float RespawnTime;
	
	void SetDefaults(string diffName)
	{
		Name = diffName;
		
		if (diffName == "Easy")
		{
			AccuracyMin = 0.1;
			AccuracyMax = 0.3;
			DamageMultiplier = 0.5;
			DamageReceivedMultiplier = 1.5;
			ReactionTime = 3.0;
			DetectionRadius = 60.0;
			MovementSpeedMultiplier = 0.8;
			Brutality = 0.2;
			MeleeChance = 0.3;
			MeleeDamage = 15.0;
			FireRate = 0.5;
			AimSpread = 8.0;
			HealthMultiplier = 0.8;
			StealthDistance = 40.0;
			StealthSpeedMultiplier = 0.5;
			DodgeChance = 0.05;
			RespawnTime = 600.0;
		}
		else if (diffName == "Hard")
		{
			AccuracyMin = 0.6;
			AccuracyMax = 0.9;
			DamageMultiplier = 1.5;
			DamageReceivedMultiplier = 0.7;
			ReactionTime = 0.5;
			DetectionRadius = 150.0;
			MovementSpeedMultiplier = 1.2;
			Brutality = 0.9;
			MeleeChance = 0.7;
			MeleeDamage = 40.0;
			FireRate = 2.0;
			AimSpread = 2.0;
			HealthMultiplier = 1.5;
			StealthDistance = 60.0;
			StealthSpeedMultiplier = 0.8;
			DodgeChance = 0.3;
			RespawnTime = 180.0;
		}
		else
		{
			AccuracyMin = 0.3;
			AccuracyMax = 0.6;
			DamageMultiplier = 1.0;
			DamageReceivedMultiplier = 1.0;
			ReactionTime = 1.5;
			DetectionRadius = 100.0;
			MovementSpeedMultiplier = 1.0;
			Brutality = 0.5;
			MeleeChance = 0.5;
			MeleeDamage = 25.0;
			FireRate = 1.0;
			AimSpread = 5.0;
			HealthMultiplier = 1.0;
			StealthDistance = 50.0;
			StealthSpeedMultiplier = 0.6;
			DodgeChance = 0.15;
			RespawnTime = 300.0;
		}
	}
};
