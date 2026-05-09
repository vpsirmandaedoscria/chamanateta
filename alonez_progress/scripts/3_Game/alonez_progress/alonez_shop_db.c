class AloneZShopPlayerData
{
    string SteamID64;
    string NamePlayer;
    int Balance;
}

class AloneZShopDB
{
    static void AddCoins(string steamId, string playerName, int amount)
    {
        if (amount <= 0) return;
        AloneZSettings s = AloneZStagesConfig.GetSettings();
        string shopDir = s.ShopPath;
        if (shopDir == "") shopDir = "$profile:FlameHost/Addons/Shop/Players/PlayerDatabase";

        if (!FileExist(shopDir))
        {
            string parts = shopDir;
            TStringArray segs = new TStringArray;
            parts.Replace("\\", "/");
            parts.Split("/", segs);
            string built = "";
            for (int si = 0; si < segs.Count(); si++)
            {
                if (si == 0)
                    built = segs.Get(si);
                else
                    built = built + "/" + segs.Get(si);
                if (!FileExist(built))
                    MakeDirectory(built);
            }
        }

        string path = shopDir + "/" + steamId + ".json";
        AloneZShopPlayerData data;

        if (FileExist(path))
        {
            JsonFileLoader<AloneZShopPlayerData>.JsonLoadFile(path, data);
        }

        if (!data)
        {
            data = new AloneZShopPlayerData();
            data.SteamID64 = steamId;
            data.NamePlayer = playerName;
            data.Balance = 0;
        }

        data.NamePlayer = playerName;
        data.Balance = data.Balance + amount;
        JsonFileLoader<AloneZShopPlayerData>.JsonSaveFile(path, data);
    }
}
