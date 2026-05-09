class AloneZStage
{
    int Units;
    int CoinsReward;
}

class AloneZCategoryConfig
{
    ref array<AloneZStage> Stages;
}

class AloneZTimeConfig
{
    ref array<AloneZStage> Stages;
    int HourlyCoinsReward;
}

class AloneZSettings
{
    string LevelUpMessage;
    string RewardMessage;
    string HourlyMessage;
    string LevelMaxMessage;
    string ShopPath;
}

class AloneZSpecificObjective
{
    string Name;
    int Units;
    int CoinsReward;
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

    static int GetHourlyCoinsReward()
    {
        if (!s_Time) Load();
        return s_Time.HourlyCoinsReward;
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
            WriteFile(path, "{\n    \"Admins\": [\n        \"7656119825xxxxxx\"\n    ]\n}");
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
        if (!cfg) { cfg = new AloneZTimeConfig(); cfg.Stages = new array<AloneZStage>(); cfg.HourlyCoinsReward = 200; }
        if (!cfg.Stages) cfg.Stages = new array<AloneZStage>();
        return cfg;
    }

    static AloneZSettings LoadSettings(string path)
    {
        if (!FileExist(path))
        {
            string def = "{\n";
            def = def + "    \"LevelUpMessage\": \"Parabens! Voce subiu para Level {level} em {category}!\",\n";
            def = def + "    \"RewardMessage\": \"Voce ganhou: {coins} coins!\",\n";
            def = def + "    \"HourlyMessage\": \"Recompensa por {hours} hora(s) jogada(s): {coins} coins!\",\n";
            def = def + "    \"LevelMaxMessage\": \"LEVEL MAX\",\n";
            def = def + "    \"ShopPath\": \"$profile:FlameHost/Addons/Shop/Players/PlayerDatabase\"\n";
            def = def + "}";
            WriteFile(path, def);
        }
        AloneZSettings cfg;
        JsonFileLoader<AloneZSettings>.JsonLoadFile(path, cfg);
        if (!cfg) cfg = new AloneZSettings();
        if (cfg.LevelUpMessage == "") cfg.LevelUpMessage = "Parabens! Voce subiu para Level {level} em {category}!";
        if (cfg.RewardMessage == "") cfg.RewardMessage = "Voce ganhou: {coins} coins!";
        if (cfg.HourlyMessage == "") cfg.HourlyMessage = "Recompensa por {hours} hora(s) jogada(s): {coins} coins!";
        if (cfg.LevelMaxMessage == "") cfg.LevelMaxMessage = "LEVEL MAX";
        if (cfg.ShopPath == "") cfg.ShopPath = "$profile:FlameHost/Addons/Shop/Players/PlayerDatabase";
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
            def = def + "            \"CoinsReward\": 1000\n";
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

    static string StageJson(int units, int coinsReward)
    {
        string s = "        {\n";
        s = s + "            \"Units\": " + units.ToString() + ",\n";
        s = s + "            \"CoinsReward\": " + coinsReward.ToString() + "\n";
        s = s + "        }";
        return s;
    }

    static string DefaultZedJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(4,  500) + ",\n";
        j = j + StageJson(10, 1000) + ",\n";
        j = j + StageJson(25, 2500) + "\n";
        j = j + "    ]\n}";
        return j;
    }

    static string DefaultAnimalJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(3, 300) + ",\n";
        j = j + StageJson(8, 800) + "\n";
        j = j + "    ]\n}";
        return j;
    }

    static string DefaultTimeJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(10, 500) + ",\n";
        j = j + StageJson(30, 1500) + ",\n";
        j = j + StageJson(60, 3000) + "\n";
        j = j + "    ],\n";
        j = j + "    \"HourlyCoinsReward\": 200\n";
        j = j + "}";
        return j;
    }

    static string DefaultDistanceJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(10,  300) + ",\n";
        j = j + StageJson(50,  1000) + ",\n";
        j = j + StageJson(100, 2500) + "\n";
        j = j + "    ]\n}";
        return j;
    }

    static string DefaultDeathsJson()
    {
        string j = "{\n    \"Stages\": [\n";
        j = j + StageJson(2, 500) + ",\n";
        j = j + StageJson(5, 1500) + "\n";
        j = j + "    ]\n}";
        return j;
    }
}
