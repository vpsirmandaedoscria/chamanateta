// AloneZ BOTS - Loot System
// Sistema de loot configuravel por dificuldade

class ABBotLoot
{
	static void EquipBot(PlayerBase botEntity, string difficulty, string loadoutName)
	{
		if (!botEntity)
			return;
		
		ABDifficultyConfig diff = ABConfig.GetDifficulty(difficulty);
		
		// Equipar com base na dificuldade e loadout
		if (loadoutName == "HeavyMilitaryLoadout")
		{
			EquipHeavyMilitary(botEntity, difficulty);
		}
		else if (loadoutName == "MilitaryLoadout")
		{
			EquipMilitary(botEntity, difficulty);
		}
		else
		{
			EquipCivilian(botEntity, difficulty);
		}
		
		// Sempre equipar faca (combate melee)
		EquipMeleeWeapon(botEntity, difficulty);
	}
	
	static void EquipCivilian(PlayerBase botEntity, string difficulty)
	{
		// Roupas civis
		AddClothing(botEntity, "TShirt_Green");
		AddClothing(botEntity, "Jeans_Blue");
		AddClothing(botEntity, "AthleticShoes_Black");
		AddClothing(botEntity, "BaseballCap_Blue");
		
		if (difficulty == "Easy")
		{
			AddWeaponWithAmmo(botEntity, "MakarovIJ70", "Mag_IJ70_8Rnd", "Ammo_380", 8);
		}
		else if (difficulty == "Medium")
		{
			AddWeaponWithAmmo(botEntity, "CZ75", "Mag_CZ75_15Rnd", "Ammo_9x19", 15);
		}
		else // Hard
		{
			AddWeaponWithAmmo(botEntity, "FNX45", "Mag_FNX45_15Rnd", "Ammo_45ACP", 15);
		}
	}
	
	static void EquipMilitary(PlayerBase botEntity, string difficulty)
	{
		// Roupas militares
		AddClothing(botEntity, "M65Jacket_Olive");
		AddClothing(botEntity, "CargoPants_Green");
		AddClothing(botEntity, "MilitaryBoots_Black");
		AddClothing(botEntity, "MilitaryBeret_Green");
		
		if (difficulty == "Easy")
		{
			AddWeaponWithAmmo(botEntity, "SKS", "", "Ammo_762x39", 10);
		}
		else if (difficulty == "Medium")
		{
			AddWeaponWithAmmo(botEntity, "AKM", "Mag_AKM_30Rnd", "Ammo_762x39", 30);
			AddClothing(botEntity, "PlateCarrierVest");
		}
		else // Hard
		{
			AddWeaponWithAmmo(botEntity, "M4A1", "Mag_M4A1_30Rnd", "Ammo_556x45", 30);
			AddClothing(botEntity, "PlateCarrierVest");
			AddClothing(botEntity, "MilitaryBeret_Red");
		}
	}
	
	static void EquipHeavyMilitary(PlayerBase botEntity, string difficulty)
	{
		// Roupas militares pesadas
		AddClothing(botEntity, "GorkaEJacket_PautRev");
		AddClothing(botEntity, "GorkaEPants_PautRev");
		AddClothing(botEntity, "MilitaryBoots_Black");
		AddClothing(botEntity, "Mich2001Helmet");
		AddClothing(botEntity, "PlateCarrierVest");
		AddClothing(botEntity, "TacticalGloves_Black");
		
		if (difficulty == "Easy")
		{
			AddWeaponWithAmmo(botEntity, "AKM", "Mag_AKM_30Rnd", "Ammo_762x39", 30);
		}
		else if (difficulty == "Medium")
		{
			AddWeaponWithAmmo(botEntity, "M4A1", "Mag_M4A1_30Rnd", "Ammo_556x45", 30);
		}
		else // Hard
		{
			float roll = Math.RandomFloat01();
			if (roll < 0.3)
				AddWeaponWithAmmo(botEntity, "SVD", "Mag_SVD_10Rnd", "Ammo_762x54", 10);
			else
				AddWeaponWithAmmo(botEntity, "M4A1", "Mag_M4A1_30Rnd", "Ammo_556x45", 30);
		}
	}
	
	static void EquipMeleeWeapon(PlayerBase botEntity, string difficulty)
	{
		if (!botEntity)
			return;
		
		string knifeType = "KitchenKnife";
		
		if (difficulty == "Medium")
			knifeType = "HuntingKnife";
		else if (difficulty == "Hard")
			knifeType = "CombatKnife";
		
		EntityAI knife = botEntity.GetInventory().CreateInInventory(knifeType);
		if (!knife)
		{
			// Se nao coube no inventario, criar na mao
			vector dropPos = botEntity.GetPosition() + Vector(0.3, 0, 0);
			knife = EntityAI.Cast(GetGame().CreateObjectEx(knifeType, dropPos, ECE_PLACE_ON_SURFACE));
		}
	}
	
	static void AddClothing(PlayerBase botEntity, string className)
	{
		if (!botEntity)
			return;
		
		EntityAI item = botEntity.GetInventory().CreateInInventory(className);
		if (item)
		{
			botEntity.GetInventory().CreateInInventory(className);
		}
	}
	
	static void AddWeaponWithAmmo(PlayerBase botEntity, string weaponClass, string magClass, string ammoClass, int ammoCount)
	{
		if (!botEntity)
			return;
		
		EntityAI weapon = botEntity.GetInventory().CreateInInventory(weaponClass);
		if (!weapon)
			return;
		
		Weapon_Base weaponBase = Weapon_Base.Cast(weapon);
		if (!weaponBase)
			return;
		
		// Adicionar magazine se especificada
		if (magClass != "")
		{
			EntityAI mag = weapon.GetInventory().CreateInInventory(magClass);
			Magazine magazine = Magazine.Cast(mag);
			if (magazine)
			{
				magazine.ServerSetAmmoCount(ammoCount);
			}
		}
		
		// Adicionar municao extra no inventario
		EntityAI extraAmmo = botEntity.GetInventory().CreateInInventory(ammoClass);
		if (extraAmmo)
		{
			Magazine extraMag = Magazine.Cast(extraAmmo);
			if (extraMag)
			{
				extraMag.ServerSetAmmoCount(ammoCount * 2);
			}
		}
	}
	
	// Drop loot quando o bot morre
	static void DropDeathLoot(PlayerBase botEntity, string difficulty)
	{
		if (!botEntity)
			return;
		
		ABLootTable lootTable = ABConfig.GetLootTable(difficulty);
		if (!lootTable || !lootTable.Items)
			return;
		
		vector basePos = botEntity.GetPosition();
		
		foreach (ABLootItem lootItem : lootTable.Items)
		{
			if (Math.RandomFloat01() > lootItem.DropChance)
				continue;
			
			int qty = Math.RandomIntInclusive(lootItem.QuantityMin, lootItem.QuantityMax);
			
			vector dropPos = basePos + Vector(Math.RandomFloat(-1.5, 1.5), 0, Math.RandomFloat(-1.5, 1.5));
			
			EntityAI item = EntityAI.Cast(GetGame().CreateObjectEx(lootItem.ClassName, dropPos, ECE_PLACE_ON_SURFACE));
			if (item)
			{
				Magazine mag = Magazine.Cast(item);
				if (mag)
					mag.ServerSetAmmoCount(qty);
				
				ABLogger.LogLoot("DeathDrop", lootItem.ClassName, qty);
			}
		}
	}
};
