enum AloneZCat
{
    ZED = 1,
    ANIMAL = 2,
    TIME = 3,
    DIST = 4,
    DEATH = 5,
    SPECIFIC = 6
}

class AloneZLog
{
    static void EnsureLogDir()
    {
        AloneZStagesConfig.EnsureDir();
    }

    static string DateStr()
    {
        int yr, mo, dy, hr, mn, sc;
        GetYearMonthDay(yr, mo, dy);
        GetHourMinuteSecond(hr, mn, sc);
        return yr.ToStringLen(4) + "-" + mo.ToStringLen(2) + "-" + dy.ToStringLen(2);
    }

    static string TimeStr()
    {
        int yr, mo, dy, hr, mn, sc;
        GetYearMonthDay(yr, mo, dy);
        GetHourMinuteSecond(hr, mn, sc);
        return hr.ToStringLen(2) + ":" + mn.ToStringLen(2) + ":" + sc.ToStringLen(2);
    }

    static void WriteLog(string logLine)
    {
        EnsureLogDir();
        string dateStr = DateStr();
        string path = AloneZStagesConfig.LOGS_DIR + "/" + dateStr + ".log";
        FileHandle f = OpenFile(path, FileMode.APPEND);
        if (f)
        {
            FPrintln(f, logLine);
            CloseFile(f);
        }
    }

    static void WriteUpLog(string playerName, string steamId, string category, int newLevel, int coinsReceived, int totalReward)
    {
        AloneZSettings s = AloneZStagesConfig.GetSettings();
        string lvlMsg = s.LevelUpMessage;
        lvlMsg.Replace("{level}", newLevel.ToString());
        lvlMsg.Replace("{category}", category);
        string rwdMsg = s.RewardMessage;
        rwdMsg.Replace("{coins}", coinsReceived.ToString());
        string line = "[" + DateStr() + " " + TimeStr() + "] " + playerName + " (" + steamId + ") " + lvlMsg + " " + rwdMsg + " totalizando " + totalReward.ToString() + " ganhos";
        WriteLog(line);
    }

    static void WriteHourLog(string playerName, string steamId, int totalHours, int coinsReceived, int totalReward)
    {
        AloneZSettings s = AloneZStagesConfig.GetSettings();
        string hrMsg = s.HourlyMessage;
        hrMsg.Replace("{hours}", totalHours.ToString());
        hrMsg.Replace("{coins}", coinsReceived.ToString());
        string line = "[" + DateStr() + " " + TimeStr() + "] " + playerName + " (" + steamId + ") " + hrMsg + " totalizando " + totalReward.ToString() + " ganhos";
        WriteLog(line);
    }
}

class AloneZProgressSrv
{
    static string CategoryName(AloneZCat cat)
    {
        switch (cat)
        {
            case AloneZCat.ZED:    return "Zumbis";
            case AloneZCat.ANIMAL: return "Animais";
            case AloneZCat.TIME:   return "Tempo";
            case AloneZCat.DIST:   return "Distancia";
            case AloneZCat.DEATH:  return "Abates";
        }
        return "Progresso";
    }

    static array<AloneZStage> GetArr(AloneZCat cat)
    {
        switch (cat)
        {
            case AloneZCat.ZED:    return AloneZStagesConfig.GetZed();
            case AloneZCat.ANIMAL: return AloneZStagesConfig.GetAnimal();
            case AloneZCat.TIME:   return AloneZStagesConfig.GetTime();
            case AloneZCat.DIST:   return AloneZStagesConfig.GetDistance();
            case AloneZCat.DEATH:  return AloneZStagesConfig.GetDeaths();
        }
        return null;
    }

    static int GetStageUnits(AloneZPlayerData d, AloneZCat cat)
    {
        int stage = 0;
        switch (cat)
        {
            case AloneZCat.ZED:    stage = d.ZedStage;      break;
            case AloneZCat.ANIMAL: stage = d.AnimalStage;   break;
            case AloneZCat.TIME:   stage = d.TimeStage;     break;
            case AloneZCat.DIST:   stage = d.DistanceStage; break;
            case AloneZCat.DEATH:  stage = d.DeathStage;    break;
        }
        array<AloneZStage> arr = GetArr(cat);
        if (!arr || arr.Count() == 0) return 10;
        if (stage >= arr.Count()) return 0;
        int idx = stage;
        if (idx >= arr.Count()) idx = arr.Count() - 1;
        int u = arr.Get(idx).Units;
        if (u <= 0) u = 10;
        return u;
    }

    static void SendMsg(PlayerBase pb, string msg)
    {
        if (!pb || !pb.GetIdentity() || msg == "") return;
        GetGame().RPCSingleParam(pb, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(msg), true, pb.GetIdentity());
    }

    static void SendNotify(PlayerBase pb, string title, string msg, int seconds = 5)
    {
        if (!pb || !pb.GetIdentity()) return;
        Param3<string, string, int> p = new Param3<string, string, int>(title, msg, seconds);
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_NOTIFY, p, true, pb.GetIdentity());
    }

    static void SendLevelUpMessage(PlayerBase pb, string category, int newLevel)
    {
        AloneZSettings s = AloneZStagesConfig.GetSettings();
        string msg = s.LevelUpMessage;
        msg.Replace("{level}", newLevel.ToString());
        msg.Replace("{category}", category);
        SendMsg(pb, msg);
    }

    static void SendCoinsRewardMessage(PlayerBase pb, int coins)
    {
        if (!pb || !pb.GetIdentity() || coins <= 0) return;
        AloneZSettings s = AloneZStagesConfig.GetSettings();
        string msg = s.RewardMessage;
        msg.Replace("{coins}", coins.ToString());
        SendMsg(pb, msg);
    }

    static void SendLevelMaxMessage(PlayerBase pb, string category)
    {
        AloneZSettings s = AloneZStagesConfig.GetSettings();
        SendMsg(pb, s.LevelMaxMessage + " — " + category);
    }

    static void GiveCoins(PlayerBase pb, string uid, int coins)
    {
        if (!pb || !pb.GetIdentity() || coins <= 0) return;
        string steamId = pb.GetIdentity().GetPlainId();
        string playerName = pb.GetIdentity().GetName();
        AloneZShopDB.AddCoins(steamId, playerName, coins);
        SendCoinsRewardMessage(pb, coins);
    }

    static void SyncAll(PlayerBase pb, string uid, AloneZPlayerData d)
    {
        int stepZ  = GetStageUnits(d, AloneZCat.ZED);
        int stepA  = GetStageUnits(d, AloneZCat.ANIMAL);
        int stepT  = GetStageUnits(d, AloneZCat.TIME);
        int stepD  = GetStageUnits(d, AloneZCat.DIST);
        int stepDe = GetStageUnits(d, AloneZCat.DEATH);

        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_SET_ZED,      new Param1<int>(d.ZedValue),      true, pb.GetIdentity());
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_SET_ANIM,     new Param1<int>(d.AnimalValue),   true, pb.GetIdentity());
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_SET_TIME,     new Param1<int>(d.TimeValue),     true, pb.GetIdentity());
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_SET_DISTANCE, new Param1<int>(d.DistanceValue), true, pb.GetIdentity());
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_SET_DEATHS,   new Param1<int>(d.DeathValue),    true, pb.GetIdentity());
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_SET_REWARD_COUNT, new Param1<int>(d.RewardCount), true, pb.GetIdentity());

        Param5<int,int,int,int,int> steps = new Param5<int,int,int,int,int>(stepZ, stepA, stepT, stepD, stepDe);
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_STEPS, steps, true, pb.GetIdentity());

        Param5<int,int,int,int,int> stages = new Param5<int,int,int,int,int>(d.ZedStage, d.AnimalStage, d.TimeStage, d.DistanceStage, d.DeathStage);
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_STAGES, stages, true, pb.GetIdentity());

        array<AloneZStage> zArr = AloneZStagesConfig.GetZed();
        array<AloneZStage> aArr = AloneZStagesConfig.GetAnimal();
        array<AloneZStage> tArr = AloneZStagesConfig.GetTime();
        array<AloneZStage> dArr = AloneZStagesConfig.GetDistance();
        array<AloneZStage> deArr = AloneZStagesConfig.GetDeaths();
        int mZ = 999; if (zArr) mZ = zArr.Count();
        int mA = 999; if (aArr) mA = aArr.Count();
        int mT = 999; if (tArr) mT = tArr.Count();
        int mD = 999; if (dArr) mD = dArr.Count();
        int mDe = 999; if (deArr) mDe = deArr.Count();

        Param5<int,int,int,int,int> maxStages = new Param5<int,int,int,int,int>(mZ, mA, mT, mD, mDe);
        GetGame().RPCSingleParam(pb, ALONEZ_RPC_SYNC_MAX_STAGES, maxStages, true, pb.GetIdentity());
    }

    static void AddProgress(PlayerBase pb, string uid, AloneZCat cat, int delta)
    {
        if (!pb || !pb.GetIdentity()) return;
        AloneZPlayerData d = AloneZPlayerDB.Load(uid);

        int value = 0;
        int stage = 0;
        array<AloneZStage> arr = GetArr(cat);

        switch (cat)
        {
            case AloneZCat.ZED:    value = d.ZedValue;      stage = d.ZedStage;      break;
            case AloneZCat.ANIMAL: value = d.AnimalValue;   stage = d.AnimalStage;   break;
            case AloneZCat.TIME:   value = d.TimeValue;     stage = d.TimeStage;     break;
            case AloneZCat.DIST:   value = d.DistanceValue; stage = d.DistanceStage; break;
            case AloneZCat.DEATH:  value = d.DeathValue;    stage = d.DeathStage;    break;
        }

        int maxStage = 999;
        if (arr) maxStage = arr.Count();

        if (stage >= maxStage)
        {
            SyncAll(pb, uid, d);
            return;
        }

        if (!arr || arr.Count() == 0)
        {
            value += delta;
        }
        else
        {
            int idx = stage;
            if (idx >= arr.Count()) idx = arr.Count() - 1;
            int stepUnits = arr.Get(idx).Units;
            if (stepUnits <= 0) stepUnits = 10;
            value += delta;
            int oldStage = stage;
            int safeLoop = 0;
            while (value >= stepUnits && safeLoop < 100)
            {
                safeLoop++;
                value -= stepUnits;

                int coins = arr.Get(idx).CoinsReward;
                if (coins > 0)
                {
                    d.RewardCount += coins;
                    GiveCoins(pb, uid, coins);
                }

                stage += 1;

                string pName = pb.GetIdentity().GetName();
                AloneZLog.WriteUpLog(pName, uid, CategoryName(cat), stage, coins, d.RewardCount);
                SendLevelUpMessage(pb, CategoryName(cat), stage);

                if (stage >= maxStage)
                {
                    value = 0;
                    SendLevelMaxMessage(pb, CategoryName(cat));
                    break;
                }
                idx = stage;
                if (idx >= arr.Count()) idx = arr.Count() - 1;
                stepUnits = arr.Get(idx).Units;
                if (stepUnits <= 0) stepUnits = 10;
            }
        }

        switch (cat)
        {
            case AloneZCat.ZED:    d.ZedValue = value;      d.ZedStage = stage;      break;
            case AloneZCat.ANIMAL: d.AnimalValue = value;   d.AnimalStage = stage;   break;
            case AloneZCat.TIME:   d.TimeValue = value;     d.TimeStage = stage;     break;
            case AloneZCat.DIST:   d.DistanceValue = value; d.DistanceStage = stage; break;
            case AloneZCat.DEATH:  d.DeathValue = value;    d.DeathStage = stage;    break;
        }
        AloneZPlayerDB.Save(uid, d);
        SyncAll(pb, uid, d);
    }

    static void GiveHourlyRewards(PlayerBase pb, string uid)
    {
        if (!pb || !pb.GetIdentity()) return;
        AloneZPlayerData d = AloneZPlayerDB.Load(uid);
        int hourlyCoins = AloneZStagesConfig.GetHourlyCoinsReward();
        if (hourlyCoins <= 0) return;

        d.TotalHours += 1;
        d.RewardCount += hourlyCoins;
        GiveCoins(pb, uid, hourlyCoins);

        AloneZSettings s = AloneZStagesConfig.GetSettings();
        string msg = s.HourlyMessage;
        msg.Replace("{hours}", d.TotalHours.ToString());
        msg.Replace("{coins}", hourlyCoins.ToString());
        SendMsg(pb, msg);

        string pName = pb.GetIdentity().GetName();
        AloneZLog.WriteHourLog(pName, uid, d.TotalHours, hourlyCoins, d.RewardCount);

        AloneZPlayerDB.Save(uid, d);
        SyncAll(pb, uid, d);
    }

    static void AddSpecificProgress(PlayerBase pb, string uid, string objName, int delta)
    {
        if (!pb || !pb.GetIdentity() || objName == "") return;
        AloneZPlayerData d = AloneZPlayerDB.Load(uid);
        array<AloneZSpecificObjective> specArr = AloneZStagesConfig.GetSpecific();

        AloneZSpecificObjective obj = null;
        for (int i = 0; i < specArr.Count(); i++)
        {
            if (specArr.Get(i).Name == objName)
            {
                obj = specArr.Get(i);
                break;
            }
        }
        if (!obj) return;

        AloneZSpecificProgress prog = null;
        for (int j = 0; j < d.Specific.Count(); j++)
        {
            if (d.Specific.Get(j).Name == objName)
            {
                prog = d.Specific.Get(j);
                break;
            }
        }
        if (!prog)
        {
            prog = new AloneZSpecificProgress();
            prog.Name = objName;
            d.Specific.Insert(prog);
        }

        int value = prog.Value;
        int stage = prog.Stage;
        value += delta;
        int objUnits = obj.Units;
        if (objUnits <= 0) objUnits = 10;
        if (value >= objUnits)
        {
            value -= objUnits;
            int coins = obj.CoinsReward;
            if (coins > 0)
            {
                d.RewardCount += coins;
                GiveCoins(pb, uid, coins);
            }
            stage += 1;
            string pName = pb.GetIdentity().GetName();
            AloneZLog.WriteUpLog(pName, uid, objName, stage, coins, d.RewardCount);
            SendLevelUpMessage(pb, objName, stage);
        }

        prog.Value = value;
        prog.Stage = stage;

        AloneZPlayerDB.Save(uid, d);
        SyncAll(pb, uid, d);
    }

    static void HandleChatCommand(PlayerBase pb, string uid, string text)
    {
        if (!pb || !pb.GetIdentity()) return;
        if (!AloneZStagesConfig.IsAdmin(uid)) return;

        string lower = text;
        lower.ToLower();

        if (lower == "!alonez reload all")
        {
            AloneZStagesConfig.Reload("all");
            SendNotify(pb, "AloneZ Progress", "Todas as configs recarregadas com sucesso!", 5);
            return;
        }

        TStringArray validNames = new TStringArray;
        validNames.Insert("zed"); validNames.Insert("animal"); validNames.Insert("time");
        validNames.Insert("distance"); validNames.Insert("deaths");
        validNames.Insert("settings"); validNames.Insert("specific"); validNames.Insert("admin");

        bool found = false;
        for (int i = 0; i < validNames.Count(); i++)
        {
            if (lower == "!alonez reload " + validNames.Get(i))
            {
                AloneZStagesConfig.Reload(validNames.Get(i));
                SendNotify(pb, "AloneZ Progress", "Config '" + validNames.Get(i) + "' recarregada com sucesso!", 5);
                found = true;
                break;
            }
        }

        if (!found && lower.IndexOf("!alonez") == 0)
        {
            SendNotify(pb, "AloneZ Progress", "Comando nao reconhecido. Use: !alonez reload all", 5);
        }
    }
}
