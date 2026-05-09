
class AloneZServerConfig
{
    int KillStepsZed;
    int KillStepsAnimal;
    int TimeSteps;
    int DistanceSteps;
    int DeathSteps;
}

class AloneZServerCfg
{
    static string ROOT_DIR  = "$profile:AloneZ";
    static string CFG_DIR   = "$profile:AloneZ/Progress";
    static string CFG_PATH  = "$profile:AloneZ/Progress/config.json";
    static ref AloneZServerConfig s_Cfg;

    static AloneZServerConfig Get()
    {
        if (!s_Cfg) Load();
        return s_Cfg;
    }

    static void EnsureDirs()
    {
        if (!FileExist(ROOT_DIR)) MakeDirectory(ROOT_DIR);
        if (!FileExist(CFG_DIR))  MakeDirectory(CFG_DIR);
    }

    static void Load()
    {
        EnsureDirs();
        if (FileExist(CFG_PATH)) JsonFileLoader<AloneZServerConfig>.JsonLoadFile(CFG_PATH, s_Cfg);
        if (!s_Cfg) { s_Cfg = new AloneZServerConfig(); s_Cfg.KillStepsZed = 10; s_Cfg.KillStepsAnimal = 10; s_Cfg.TimeSteps = 10; s_Cfg.DistanceSteps = 10; s_Cfg.DeathSteps = 10; Save(); }
    }

    static void Save()
    {
        EnsureDirs();
        JsonFileLoader<AloneZServerConfig>.JsonSaveFile(CFG_PATH, s_Cfg);
    }
}
