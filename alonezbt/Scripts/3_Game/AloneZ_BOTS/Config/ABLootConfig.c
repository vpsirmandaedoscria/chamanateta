class ABLootTable
{
	string Difficulty;
	ref array<ref ABLootItem> Items;
	
	void ABLootTable()
	{
		Items = new array<ref ABLootItem>();
	}
	
	void SetDefaults(string diff)
	{
		Difficulty = diff;
		Items = new array<ref ABLootItem>();
		
		if (diff == "Easy")
		{
			AddItem("BandageDressing", 1, 2, 0.8);
			AddItem("SodaCan_Cola", 1, 1, 0.5);
			AddItem("Apple", 1, 2, 0.6);
			AddItem("Rag", 2, 4, 0.7);
			AddItem("Ammo_9x19", 5, 15, 0.3);
			AddItem("MakarovIJ70", 1, 1, 0.1);
			AddItem("KitchenKnife", 1, 1, 0.4);
		}
		else if (diff == "Hard")
		{
			AddItem("Morphine", 1, 2, 0.5);
			AddItem("Epinephrine", 1, 1, 0.3);
			AddItem("BandageDressing", 2, 4, 0.8);
			AddItem("Ammo_762x54", 10, 40, 0.6);
			AddItem("Ammo_556x45", 20, 60, 0.6);
			AddItem("Mag_M4A1_30Rnd", 1, 3, 0.4);
			AddItem("M4A1", 1, 1, 0.2);
			AddItem("SVD", 1, 1, 0.08);
			AddItem("PlateCarrierVest", 1, 1, 0.25);
			AddItem("NVGHeadstrap", 1, 1, 0.1);
			AddItem("CombatKnife", 1, 1, 0.5);
			AddItem("HandGrenade", 1, 2, 0.15);
		}
		else
		{
			AddItem("BandageDressing", 1, 3, 0.7);
			AddItem("Morphine", 1, 1, 0.3);
			AddItem("TacticalBaconCan", 1, 2, 0.5);
			AddItem("Ammo_762x39", 10, 30, 0.5);
			AddItem("Ammo_556x45", 10, 30, 0.5);
			AddItem("Mag_AKM_30Rnd", 1, 2, 0.3);
			AddItem("AKM", 1, 1, 0.15);
			AddItem("CombatKnife", 1, 1, 0.4);
			AddItem("PlateCarrierVest", 1, 1, 0.1);
		}
	}
	
	void AddItem(string className, int qtyMin, int qtyMax, float dropChance)
	{
		ref ABLootItem item = new ABLootItem();
		item.ClassName = className;
		item.QuantityMin = qtyMin;
		item.QuantityMax = qtyMax;
		item.DropChance = dropChance;
		Items.Insert(item);
	}
};

class ABLootItem
{
	string ClassName;
	int QuantityMin;
	int QuantityMax;
	float DropChance;
};
