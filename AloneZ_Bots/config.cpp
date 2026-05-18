class CfgPatches
{
    class AloneZ_Bots
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Weapons_Firearms", "DZ_Characters"};
    };
};

class CfgMods
{
    class AloneZ_Bots
    {
        type = "mod";
        author = "AloneZ Team";
        
        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"AloneZ_Bots/Scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"AloneZ_Bots/Scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"AloneZ_Bots/Scripts/5_Mission"};
            };
        };
    };
};
