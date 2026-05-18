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

class CfgSoundShaders
{
    class AloneZ_Shot_SoundShader
    {
        samples[] = {{"DZ\sounds\weapons\firearms\AKM\AKM_close",1}};
        volume = 1.0;
        range = 200;
        rangeCurve[] = {0,1,200,0};
    };
};

class CfgSoundSets
{
    class AloneZ_Shot_SoundSet
    {
        soundShaders[] = {"AloneZ_Shot_SoundShader"};
        volumeFactor = 1.0;
        frequencyFactor = 1.0;
        spatial = 1;
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
