class ABBotLoot
{
	static void EquipBot(PlayerBase botEntity, string difficulty, string loadoutName)
	{
		if (!botEntity)
			return;
		
		if (loadoutName == "HeavyMilitaryLoadout")
			EquipHeavyMilitary(botEntity, difficulty);
		else if (loadoutName == "MilitaryLoadout")
			EquipMilitary(botEntity, difficulty);
		else
			EquipCivilian(botEntity, difficulty);
		
		EquipMeleeWeapon(botEntity, difficulty);
	}
	
	static void EquipCivilian(PlayerBase botEntity, string difficulty)
	{
		botEntity.GetInventory().CreateInInventory("TShirt_Green");
		botEntity.GetInventory().CreateInInventory("Jeans_Blue");
		botEntity.GetInventory().CreateInInventory("AthleticShoes_Black");
		botEntity.GetInventory().CreateInInventory("BaseballCap_Blue");
		
		if (difficulty == "Easy")
			AddWeaponWithAmmo(botEntity, "MakarovIJ70", "Mag_IJ70_8Rnd", "Ammo_380", 8);
		else if (difficulty == "Medium")
			AddWeaponWithAmmo(botEntity, "CZ75", "Mag_CZ75_15Rnd", "Ammo_9x19", 15);
		else
			AddWeaponWithAmmo(botEntity, "FNX45", "Mag_FNX45_15Rnd", "Ammo_45ACP", 15);
	}
	
	static void EquipMilitary(PlayerBase botEntity, string difficulty)
	{
		botEntity.GetInventory().CreateInInventory("M65Jacket_Olive");
		botEntity.GetInventory().CreateInInventory("CargoPants_Green");
		botEntity.GetInventory().CreateInInventory("MilitaryBoots_Black");
		botEntity.GetInventory().CreateInInventory("MilitaryBeret_Green");
		
		if (difficulty == "Easy")
		{
			AddWeaponWithAmmo(botEntity, "SKS", "", "Ammo_762x39", 10);
		}
		else if (difficulty == "Medium")
		{
			AddWeaponWithAmmo(botEntity, "AKM", "Mag_AKM_30Rnd", "Ammo_762x39", 30);
			botEntity.GetInventory().CreateInInventory("PlateCarrierVest");
		}
		else
		{
			AddWeaponWithAmmo(botEntity, "M4A1", "Mag_M4A1_30Rnd", "Ammo_556x45", 30);
			botEntity.GetInventory().CreateInInventory("PlateCarrierVest");
			botEntity.GetInventory().CreateInInventory("MilitaryBeret_Red");
		}
	}
	
	static void EquipHeavyMilitary(PlayerBase botEntity, string difficulty)
	{
		botEntity.GetInventory().CreateInInventory("GorkaEJacket_PautRev");
		botEntity.GetInventory().CreateInInventory("GorkaEPants_PautRev");
		botEntity.GetInventory().CreateInInventory("MilitaryBoots_Black");
		botEntity.GetInventory().CreateInInventory("Mich2001Helmet");
		botEntity.GetInventory().CreateInInventory("PlateCarrierVest");
		botEntity.GetInventory().CreateInInventory("TacticalGloves_Black");
		
		if (difficulty == "Easy")
			AddWeaponWithAmmo(botEntity, "AKM", "Mag_AKM_30Rnd", "Ammo_762x39", 30);
		else if (difficulty == "Medium")
			AddWeaponWithAmmo(botEntity, "M4A1", "Mag_M4A1_30Rnd", "Ammo_556x45", 30);
		else
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
		
		botEntity.GetInventory().CreateInInventory(knifeType);
	}
	
	static void AddWeaponWithAmmo(PlayerBase botEntity, string weaponClass, string magClass, string ammoClass, int ammoCount)
	{
		if (!botEntity)
			return;
		
		EntityAI weapon = botEntity.GetInventory().CreateInInventory(weaponClass);
		if (!weapon)
			return;
		
		if (magClass != "")
		{
			EntityAI mag = weapon.GetInventory().CreateInInventory(magClass);
			Magazine magazine = Magazine.Cast(mag);
			if (magazine)
				magazine.ServerSetAmmoCount(ammoCount);
		}
		
		EntityAI extraAmmo = botEntity.GetInventory().CreateInInventory(ammoClass);
		if (extraAmmo)
		{
			Magazine extraMag = Magazine.Cast(extraAmmo);
			if (extraMag)
				extraMag.ServerSetAmmoCount(ammoCount * 2);
		}
	}
	
	static void DropDeathLoot(PlayerBase botEntity, string difficulty)
	{
		if (!botEntity)
			return;
		
		ABLootTable lootTable = ABConfig.GetLootTable(difficulty);
		if (!lootTable || !lootTable.Items)
			return;
		
		vector basePos = botEntity.GetPosition();
		
		for (int i = 0; i < lootTable.Items.Count(); i++)
		{
			ABLootItem lootItem = lootTable.Items[i];
			
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
