class CfgPatches
{
	class alonezbt
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters", "DZ_Weapons_Firearms", "DZ_Weapons_Melee"};
	};
};

class CfgMods
{
	class alonezbt
	{
		dir = "alonezbt";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "AloneZ BOTS";
		credits = "AloneZ";
		author = "AloneZ";
		authorID = "";
		version = "1.0.0";
		extra = 0;
		type = "mod";
		
		dependencies[] = {"Game", "World", "Mission"};
		
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"alonezbt/Scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"alonezbt/Scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"alonezbt/Scripts/5_Mission"};
			};
		};
	};
};

class CfgVehicles
{
	class SurvivorM_Mirek;
	class AB_SurvivorM_Mirek: SurvivorM_Mirek {};
	
	class SurvivorM_Denis;
	class AB_SurvivorM_Denis: SurvivorM_Denis {};
	
	class SurvivorM_Boris;
	class AB_SurvivorM_Boris: SurvivorM_Boris {};
	
	class SurvivorM_Cyril;
	class AB_SurvivorM_Cyril: SurvivorM_Cyril {};
	
	class SurvivorM_Elias;
	class AB_SurvivorM_Elias: SurvivorM_Elias {};
	
	class SurvivorM_Francis;
	class AB_SurvivorM_Francis: SurvivorM_Francis {};
	
	class SurvivorM_Guo;
	class AB_SurvivorM_Guo: SurvivorM_Guo {};
	
	class SurvivorM_Hassan;
	class AB_SurvivorM_Hassan: SurvivorM_Hassan {};
	
	class SurvivorM_Indar;
	class AB_SurvivorM_Indar: SurvivorM_Indar {};
	
	class SurvivorM_Jose;
	class AB_SurvivorM_Jose: SurvivorM_Jose {};
	
	class SurvivorM_Kaito;
	class AB_SurvivorM_Kaito: SurvivorM_Kaito {};
	
	class SurvivorM_Lewis;
	class AB_SurvivorM_Lewis: SurvivorM_Lewis {};
	
	class SurvivorM_Manua;
	class AB_SurvivorM_Manua: SurvivorM_Manua {};
	
	class SurvivorM_Niki;
	class AB_SurvivorM_Niki: SurvivorM_Niki {};
	
	class SurvivorM_Oliver;
	class AB_SurvivorM_Oliver: SurvivorM_Oliver {};
	
	class SurvivorM_Peter;
	class AB_SurvivorM_Peter: SurvivorM_Peter {};
	
	class SurvivorM_Quinn;
	class AB_SurvivorM_Quinn: SurvivorM_Quinn {};
	
	class SurvivorM_Rolf;
	class AB_SurvivorM_Rolf: SurvivorM_Rolf {};
	
	class SurvivorM_Seth;
	class AB_SurvivorM_Seth: SurvivorM_Seth {};
	
	class SurvivorM_Taiki;
	class AB_SurvivorM_Taiki: SurvivorM_Taiki {};
	
	class SurvivorF_Linda;
	class AB_SurvivorF_Linda: SurvivorF_Linda {};
	
	class SurvivorF_Maria;
	class AB_SurvivorF_Maria: SurvivorF_Maria {};
	
	class SurvivorF_Frida;
	class AB_SurvivorF_Frida: SurvivorF_Frida {};
	
	class SurvivorF_Gabi;
	class AB_SurvivorF_Gabi: SurvivorF_Gabi {};
	
	class SurvivorF_Helga;
	class AB_SurvivorF_Helga: SurvivorF_Helga {};
	
	class SurvivorF_Irena;
	class AB_SurvivorF_Irena: SurvivorF_Irena {};
	
	class SurvivorF_Judy;
	class AB_SurvivorF_Judy: SurvivorF_Judy {};
	
	class SurvivorF_Keiko;
	class AB_SurvivorF_Keiko: SurvivorF_Keiko {};
	
	class SurvivorF_Eva;
	class AB_SurvivorF_Eva: SurvivorF_Eva {};
	
	class SurvivorF_Naomi;
	class AB_SurvivorF_Naomi: SurvivorF_Naomi {};
	
	class SurvivorF_Baty;
	class AB_SurvivorF_Baty: SurvivorF_Baty {};
};
