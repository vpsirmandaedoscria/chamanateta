class AloneZSpecificProgress
{
    string Name;
    int Value;
    int Stage;
    void AloneZSpecificProgress()
    {
        Value = 0;
        Stage = 0;
    }
}

class AloneZPlayerData
{
    int ZedStage;      int ZedValue;
    int AnimalStage;   int AnimalValue;
    int TimeStage;     int TimeValue;
    int DistanceStage; int DistanceValue;
    int DeathStage;    int DeathValue;
    int RewardCount;
    int TotalHours;
    ref array<AloneZSpecificProgress> Specific;

    void AloneZPlayerData()
    {
        ZedStage = 0; AnimalStage = 0; TimeStage = 0; DistanceStage = 0; DeathStage = 0;
        ZedValue = 0; AnimalValue = 0; TimeValue = 0; DistanceValue = 0; DeathValue = 0;
        RewardCount = 0;
        TotalHours = 0;
        Specific = new array<AloneZSpecificProgress>();
    }
}

class AloneZPlayerDB
{
    static string DIR = "\$profile:AloneZ/Progress/players";
    static bool s_DirReady = false;

    static void EnsureDir()
    {
        if (s_DirReady) return;
        if (!FileExist("\$profile:AloneZ")) MakeDirectory("\$profile:AloneZ");
        if (!FileExist("\$profile:AloneZ/Progress")) MakeDirectory("\$profile:AloneZ/Progress");
        if (!FileExist(DIR)) MakeDirectory(DIR);
        s_DirReady = true;
    }

    static string PathFor(string uid)
    {
        return DIR + "/" + uid + ".json";
    }

    static AloneZPlayerData Load(string uid)
    {
        EnsureDir();
        AloneZPlayerData d;
        string p = PathFor(uid);
        if (FileExist(p)) JsonFileLoader<AloneZPlayerData>.JsonLoadFile(p, d);
        if (!d) d = new AloneZPlayerData();
        return d;
    }

    static void Save(string uid, AloneZPlayerData d)
    {
        EnsureDir();
        JsonFileLoader<AloneZPlayerData>.JsonSaveFile(PathFor(uid), d);
    }
}

