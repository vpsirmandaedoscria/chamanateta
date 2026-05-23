class CfgPatches
{
	class AloneZ_Progress
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Characters",
			"DZ_Animals"
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
