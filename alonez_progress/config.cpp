class CfgPatches
{
	class AloneZ_Progress
	{
		units[]=
		{
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Backpacks",
			"DZ_Characters_Glasses",
			"DZ_Characters_Gloves",
			"DZ_Characters_Headgear",
			"DZ_Characters_Heads",
			"DZ_Characters_Pants",
			"DZ_Characters_Masks",
			"DZ_Characters_Shoes",
			"DZ_Characters_Tops",
			"DZ_Characters_Vests",
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Characters_Belts",
			"DZ_Gear_Containers",
			"DZ_Gear_Camping",
			"DZ_Gear_Tools",						
			"DZ_Gear_Drinks",			
			"DZ_Gear_Optics",
			"DZ_Weapons_Melee",
			"DZ_Gear_Food",
			"DZ_Animals",
			"DZ_Gear_Crafting",
			"DZ_Gear_Consumables",
			"DZ_Weapons_Lights",
			"DZ_Vehicles_Wheeled",
			"DZ_Gear_Camping",
			"DZ_Gear_Traps",
			"DZ_Sounds_Weapons",
			"DZ_Weapons_Shotguns_MP133",
			"DZ_Sounds_Effects"
		};
	};
};

class CfgMods
{
	class AloneZ_Progress
	{
		dir="AloneZ_Progress";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="ALONE Z PROGRESS";
		credits="MisterRico";
		author="AloneZ";
		authorID="";
		version="1.0";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"AloneZ_Progress/scripts/3_Game/alonez_progress"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"AloneZ_Progress/scripts/4_World/classes/useractionscomponent/actions/single",
					"AloneZ_Progress/scripts/4_World/alonez_agents",
					"AloneZ_Progress/scripts/4_World/alonez_progress"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"AloneZ_Progress/scripts/5_Mission/alonez_progress"
				};
			};
		};
	};
};
