class CfgPatches
{
	class alonezbt
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters", "DZ_Weapons_Firearms", "DZ_Weapons_Melee", "DayZExpansion_AI_Animations", "DayZExpansion_Sounds_AI"};
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
