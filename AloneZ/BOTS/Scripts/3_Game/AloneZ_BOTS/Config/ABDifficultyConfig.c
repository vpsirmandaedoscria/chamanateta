// AloneZ BOTS - Difficulty Configuration
class ABDifficultyConfig
{
	string Name;
	
	// Precisao de tiro (0.0 = nunca acerta, 1.0 = sempre acerta)
	float AccuracyMin;
	float AccuracyMax;
	
	// Dano que o bot causa
	float DamageMultiplier;
	
	// Dano que o bot recebe
	float DamageReceivedMultiplier;
	
	// Velocidade de reacao em segundos (quanto menor, mais rapido)
	float ReactionTime;
	
	// Distancia maxima de deteccao de player
	float DetectionRadius;
	
	// Velocidade de movimento (multiplicador)
	float MovementSpeedMultiplier;
	
	// Brutalidade: chance de perseguir player ate a morte (0.0-1.0)
	float Brutality;
	
	// Chance de usar faca quando perto (0.0-1.0)
	float MeleeChance;
	
	// Dano da faca
	float MeleeDamage;
	
	// Taxa de disparo (tiros por segundo)
	float FireRate;
	
	// Dispersao do tiro (graus de desvio)
	float AimSpread;
	
	// Vida do bot (multiplicador do HP base)
	float HealthMultiplier;
	
	// Distancia para iniciar modo furtivo
	float StealthDistance;
	
	// Velocidade no modo furtivo (multiplicador)
	float StealthSpeedMultiplier;
	
	// Chance de desviar de tiros inimigos (0.0-1.0)
	float DodgeChance;
	
	// Tempo de respawn em segundos (-1 = nao respawna)
	float RespawnTime;
	
	void SetDefaults(string difficultyName)
	{
		Name = difficultyName;
		
		if (difficultyName == "Easy")
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
		else if (difficultyName == "Medium")
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
		else // Hard
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
	}
};
