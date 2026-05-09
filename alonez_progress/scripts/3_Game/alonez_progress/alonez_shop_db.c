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
