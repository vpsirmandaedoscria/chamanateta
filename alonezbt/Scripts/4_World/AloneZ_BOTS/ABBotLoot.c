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
			AddWeaponToHands(botEntity, "MakarovIJ70", "Mag_IJ70_8Rnd", 8);
		else if (difficulty == "Medium")
			AddWeaponToHands(botEntity, "CZ75", "Mag_CZ75_15Rnd", 15);
		else
			AddWeaponToHands(botEntity, "FNX45", "Mag_FNX45_15Rnd", 15);
	}
	
	static void EquipMilitary(PlayerBase botEntity, string difficulty)
	{
		botEntity.GetInventory().CreateInInventory("M65Jacket_Olive");
		botEntity.GetInventory().CreateInInventory("CargoPants_Green");
		botEntity.GetInventory().CreateInInventory("MilitaryBoots_Black");
		botEntity.GetInventory().CreateInInventory("MilitaryBeret_Green");
		
		if (difficulty == "Easy")
		{
			AddWeaponToHands(botEntity, "SKS", "", 10);
		}
		else if (difficulty == "Medium")
		{
			AddWeaponToHands(botEntity, "AKM", "Mag_AKM_30Rnd", 30);
			botEntity.GetInventory().CreateInInventory("PlateCarrierVest");
		}
		else
		{
			AddWeaponToHands(botEntity, "M4A1", "Mag_M4A1_30Rnd", 30);
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
			AddWeaponToHands(botEntity, "AKM", "Mag_AKM_30Rnd", 30);
		else if (difficulty == "Medium")
			AddWeaponToHands(botEntity, "M4A1", "Mag_M4A1_30Rnd", 30);
		else
		{
			float roll = Math.RandomFloat01();
			if (roll < 0.3)
				AddWeaponToHands(botEntity, "SVD", "Mag_SVD_10Rnd", 10);
			else
				AddWeaponToHands(botEntity, "M4A1", "Mag_M4A1_30Rnd", 30);
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
	
	static void AddWeaponToHands(PlayerBase botEntity, string weaponClass, string magClass, int ammoCount)
	{
		if (!botEntity)
			return;
		
		EntityAI weapon = botEntity.GetHumanInventory().CreateInHands(weaponClass);
		if (!weapon)
		{
			weapon = botEntity.GetInventory().CreateInInventory(weaponClass);
			if (!weapon)
				return;
		}
		
		if (magClass != "")
		{
			EntityAI mag = weapon.GetInventory().CreateAttachment(magClass);
			if (!mag)
				mag = weapon.GetInventory().CreateInInventory(magClass);
			
			Magazine magazine = Magazine.Cast(mag);
			if (magazine)
				magazine.ServerSetAmmoCount(ammoCount);
		}
		
		Weapon_Base wpn = Weapon_Base.Cast(weapon);
		if (wpn)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DelayedChamber, 1500, false, wpn, botEntity);
		}
	}
	
	static void DelayedChamber(Weapon_Base wpn, PlayerBase botEntity)
	{
		if (!wpn || !botEntity)
			return;
		
		int mi = wpn.GetCurrentMuzzle();
		if (wpn.IsChamberEmpty(mi))
		{
			Magazine mag = Magazine.Cast(wpn.GetMagazine(mi));
			if (mag && mag.GetAmmoCount() > 0)
			{
				wpn.ProcessWeaponEvent(new WeaponEventMechanism(botEntity));
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DelayedChamberRetry, 1000, false, wpn, botEntity);
			}
		}
	}
	
	static void DelayedChamberRetry(Weapon_Base wpn, PlayerBase botEntity)
	{
		if (!wpn || !botEntity)
			return;
		
		int mi = wpn.GetCurrentMuzzle();
		if (wpn.IsChamberEmpty(mi))
		{
			wpn.ProcessWeaponEvent(new WeaponEventMechanism(botEntity));
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
				Magazine dropMag = Magazine.Cast(item);
				if (dropMag)
					dropMag.ServerSetAmmoCount(qty);
				
				ABLogger.LogLoot("DeathDrop", lootItem.ClassName, qty);
			}
		}
	}
};
