class AloneZStageReward
{
    string Item;
    int Count;
    int Quantity;
}

class AloneZStage
{
    int Units;
    ref array<AloneZStageReward> Rewards;
}

class AloneZCategoryConfig
{
    ref array<AloneZStage> Stages;
}

class AloneZTimeConfig
{
    ref array<AloneZStage> Stages;
    ref array<AloneZStageReward> HourlyRewards;
}

class AloneZSettings
{
    string LevelUpMessage;
    string RewardMessage;
    string HourlyMessage;
    string LevelMaxMessage;
}

class AloneZSpecificObjective
{
    string Name;
    int Units;
    ref array<AloneZStageReward> Rewards;
}

class AloneZSpecificConfig
{
    ref array<AloneZSpecificObjective> Objectives;
}

class AloneZAdminConfig
{
    ref array<string> Admins;
}

class AloneZStagesConfig
{
    static string DIR  = "$profile:AloneZ/Progress";
    static string LOGS_DIR = "$profile:AloneZ/Progress/Logs";
    static bool s_DirReady = false;

    static ref AloneZCategoryConfig s_Zed;
    static ref AloneZCategoryConfig s_Animal;
    static ref AloneZTimeConfig     s_Time;
    static ref AloneZCategoryConfig s_Distance;
    static ref AloneZCategoryConfig s_Deaths;
    static ref AloneZSettings       s_Settings;
    static ref AloneZSpecificConfig s_Specific;
    static ref AloneZAdminConfig    s_Admin;

    static void EnsureDir()
    {
        if (s_DirReady) return;
        if (!FileExist("$profile:AloneZ")) MakeDirectory("$profile:AloneZ");
        if (!FileExist(DIR)) MakeDirectory(DIR);
        if (!FileExist(LOGS_DIR)) MakeDirectory(LOGS_DIR);
        s_DirReady = true;
    }

    static AloneZSettings GetSettings()
    {
        if (!s_Settings) Load();
        return s_Settings;
    }

    static array<AloneZStageReward> GetHourlyRewards()
    {
        if (!s_Time) Load();
        if (!s_Time.HourlyRewards) return new array<AloneZStageReward>();
        return s_Time.HourlyRewards;
    }

    static array<AloneZStage> GetZed()    { if (!s_Zed) Load(); return s_Zed.Stages; }
    static array<AloneZStage> GetAnimal() { if (!s_Animal) Load(); return s_Animal.Stages; }
    static array<AloneZStage> GetTime()   { if (!s_Time) Load(); return s_Time.Stages; }
    static array<AloneZStage> GetDistance(){ if (!s_Distance) Load(); return s_Distance.Stages; }
    static array<AloneZStage> GetDeaths() { if (!s_Deaths) Load(); return s_Deaths.Stages; }
    static array<AloneZSpecificObjective> GetSpecific() { if (!s_Specific) Load(); return s_Specific.Objectives; }

    static bool IsAdmin(string steamId)
    {
        if (!s_Admin) LoadAdmin();
        for (int i = 0; i < s_Admin.Admins.Count(); i++)
        {
            if (s_Admin.Admins.Get(i) == steamId) return true;
        }
        return false;
    }

    static void LoadAdmin()
    {
        EnsureDir();
        string path = DIR + "/admin.json";
        if (!FileExist(path))
        {
            WriteFile(path, "{\n    \"Admins\": [\n        \"76561198251007370\"\n    ]\n}");
        }
        JsonFileLoader<AloneZAdminConfig>.JsonLoadFile(path, s_Admin);
        if (!s_Admin) { s_Admin = new AloneZAdminConfig(); s_Admin.Admins = new array<string>(); }
        if (!s_Admin.Admins) s_Admin.Admins = new array<string>();
    }

    static void Load()
    {
        EnsureDir();
        s_Zed      = LoadCategory(DIR + "/zed.json",      DefaultZedJson());
        s_Animal   = LoadCategory(DIR + "/animal.json",    DefaultAnimalJson());
        s_Time     = LoadTimeConfig(DIR + "/time.json");
        s_Distance = LoadCategory(DIR + "/distance.json",  DefaultDistanceJson());
        s_Deaths   = LoadCategory(DIR + "/deaths.json",    DefaultDeathsJson());
        s_Settings = LoadSettings(DIR + "/settings.json");
        s_Specific = LoadSpecific(DIR + "/specific.json");
        LoadAdmin();
    }

    static void Reload(string which)
    {
        EnsureDir();
        if (which == "all")     { Load(); return; }
        if (which == "zed")      s_Zed      = LoadCategory(DIR + "/zed.json",      DefaultZedJson());
        if (which == "animal")   s_Animal   = LoadCategory(DIR + "/animal.json",    DefaultAnimalJson());
        if (which == "time")     s_Time     = LoadTimeConfig(DIR + "/time.json");
        if (which == "distance") s_Distance = LoadCategory(DIR + "/distance.json",  DefaultDistanceJson());
        if (which == "deaths")   s_Deaths   = LoadCategory(DIR + "/deaths.json",    DefaultDeathsJson());
        if (which == "settings") s_Settings = LoadSettings(DIR + "/settings.json");
        if (which == "specific") s_Specific = LoadSpecific(DIR + "/specific.json");
        if (which == "admin")    LoadAdmin();
    }

    static void WriteFile(string path, string content)
    {
        FileHandle f = OpenFile(path, FileMode.WRITE);
        if (f)
        {
            FPrint(f, content);
            CloseFile(f);
        }
    }

    static AloneZCategoryConfig LoadCategory(string path, string defaultJson)
    {
        if (!FileExist(path))
        {
            WriteFile(path, defaultJson);
        }
        AloneZCategoryConfig cfg;
        JsonFileLoader<AloneZCategoryConfig>.JsonLoadFile(path, cfg);
        if (!cfg) { cfg = new AloneZCategoryConfig(); cfg.Stages = new array<AloneZStage>(); }
        if (!cfg.Stages) cfg.Stages = new array<AloneZStage>();
        for (int i = 0; i < cfg.Stages.Count(); i++)
        {
            AloneZStage st = cfg.Stages.Get(i);
            if (!st) continue;
            if (!st.Rewards) st.Rewards = new array<AloneZStageReward>();
        }
        return cfg;
    }

    static AloneZTimeConfig LoadTimeConfig(string path)
    {
        if (!FileExist(path))
        {
            WriteFile(path, DefaultTimeJson());
        }
        AloneZTimeConfig cfg;
        JsonFileLoader<AloneZTimeConfig>.JsonLoadFile(path, cfg);
        if (!cfg) { cfg = new AloneZTimeConfig(); cfg.Stages = new array<AloneZStage>(); cfg.HourlyRewards = new array<AloneZStageReward>(); }
        if (!cfg.Stages) cfg.Stages = new array<AloneZStage>();
        if (!cfg.HourlyRewards) cfg.HourlyRewards = new array<AloneZStageReward>();
        for (int i = 0; i < cfg.Stages.Count(); i++)
        {
            AloneZStage st = cfg.Stages.Get(i);
            if (!st) continue;
            if (!st.Rewards) st.Rewards = new array<AloneZStageReward>();
        }
        return cfg;
    }

    static AloneZSettings LoadSettings(string path)
    {
        if (!FileExist(path))
        {
            string def = "{\n";
            def = def + "    \"LevelUpMessage\": \"Parabens! Voce subiu para Level {level} em {category}!\",\n";
            def = def + "    \"RewardMessage\": \"Voce ganhou: {item} x{count}\",\n";
            def = def + "    \"HourlyMessage\": \"Recompensa por {hours} hora(s) jogada(s)!\",\n";
            def = def + "    \"LevelMaxMessage\": \"LEVEL MAX\"\n";
            def = def + "}";
            WriteFile(path, def);
        }
        AloneZSettings cfg;
        JsonFileLoader<AloneZSettings>.JsonLoadFile(path, cfg);
        if (!cfg) cfg = new AloneZSettings();
        if (cfg.LevelUpMessage == "") cfg.LevelUpMessage = "Parabens! Voce subiu para Level {level} em {category}!";
        if (cfg.RewardMessage == "") cfg.RewardMessage = "Voce ganhou: {item} x{count}";
        if (cfg.HourlyMessage == "") cfg.HourlyMessage = "Recompensa por {hours} hora(s) jogada(s)!";
        if (cfg.LevelMaxMessage == "") cfg.LevelMaxMessage = "LEVEL MAX";
        return cfg;
    }

    static AloneZSpecificConfig LoadSpecific(string path)
    {
        if (!FileExist(path))
        {
            string def = "{\n";
            def = def + "    \"Objectives\": [\n";
            def = def + "        {\n";
            def = def + "            \"Name\": \"Animal_UrsusArctos\",\n";
            def = def + "            \"Units\": 1,\n";
            def = def + "            \"Rewards\": [\n";
            def = def + "                {\n";
            def = def + "                    \"Item\": \"BearPelt\",\n";
            def = def + "                    \"Count\": 1,\n";
            def = def + "                    \"Quantity\": 0\n";
            def = def + "                }\n";
            def = def + "            ]\n";
            def = def + "        }\n";
            def = def + "    ]\n";
            def = def + "}";
            WriteFile(path, def);
        }
        AloneZSpecificConfig cfg;
        JsonFileLoader<AloneZSpecificConfig>.JsonLoadFile(path, cfg);
        if (!cfg) { cfg = new AloneZSpecificConfig(); cfg.Objectives = new array<AloneZSpecificObjective>(); }
        if (!cfg.Objectives) cfg.Objectives = new array<AloneZSpecificObjective>();
        return cfg;
    }

    static string RewardJson(string item, int count, int quantity)
    {
        string s = "            {\n";
        s = s + "                \"Item\": \"" + item + "\",\n";
        s = s + "                \"Count\": " + count.ToString() + ",\n";
        s = s + "                \"Quantity\": " + quantity.ToString() + "\n";
        s = s + "            }";
        return s;
    }

    static string StageJson(int units, string rewardsBlock)
    {
        string s = "        {\n";
        s = s + "            \"Units\": " + units.ToString() + ",\n";
        s = s + "            \"Rewards\": [\n";
        s = s + rewardsBlock + "\n";
        s = s + "            ]\n";
        s = s + "        }";
        return s;
    }

    static string DefaultZedJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(4,  RewardJson("BandageDressing", 2, 0)) + ",\n";
        j = j + StageJson(10, RewardJson("Rag", 4, 4)) + ",\n";
        j = j + StageJson(25, RewardJson("SodaCan_Cola", 1, 0)) + "\n";
        j = j + "    ]\n}";
        return j;
    }

    static string DefaultAnimalJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(3, RewardJson("BandageDressing", 1, 0)) + ",\n";
        j = j + StageJson(8, RewardJson("Rag", 2, 4)) + "\n";
        j = j + "    ]\n}";
        return j;
    }

    static string DefaultTimeJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(10, RewardJson("BandageDressing", 1, 0)) + ",\n";
        j = j + StageJson(30, RewardJson("Rag", 2, 4)) + ",\n";
        j = j + StageJson(60, RewardJson("SodaCan_Cola", 1, 0)) + "\n";
        j = j + "    ],\n";
        j = j + "    \"HourlyRewards\": [\n";
        j = j + RewardJson("BandageDressing", 2, 0) + ",\n";
        j = j + RewardJson("SodaCan_Cola", 1, 0) + "\n";
        j = j + "    ]\n";
        j = j + "}";
        return j;
    }

    static string DefaultDistanceJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(10,  RewardJson("BandageDressing", 1, 0)) + ",\n";
        j = j + StageJson(50,  RewardJson("Rag", 2, 4)) + ",\n";
        j = j + StageJson(100, RewardJson("SodaCan_Cola", 1, 0)) + "\n";
        j = j + "    ]\n}";
        return j;
    }

    static string DefaultDeathsJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(2, RewardJson("BandageDressing", 1, 0)) + ",\n";
        j = j + StageJson(5, RewardJson("Rag", 3, 4)) + "\n";
        j = j + "    ]\n}";
        return j;
    }
}
