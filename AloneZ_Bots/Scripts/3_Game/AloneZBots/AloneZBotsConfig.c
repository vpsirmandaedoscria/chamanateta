// ============================================================================
// AloneZ Bots — Sistema de Configuracao
// Carrega e parseia JSONs de $profile:AloneZ/Bots/Config/
// ============================================================================

static const string ALONEZ_VERSION = "1.0.0";

// --- Classes de dados para configuracao ---

class AloneZAccuracySettings
{
    float GlobalMinAccuracy = 0.1;
    float GlobalMaxAccuracy = 0.9;
    int AccuracyScaleWithDistance = 1;
    float AccuracyDistanceFalloff = 0.005;
    float HeadshotChance = 0.05;
    int BurstFireEnabled = 1;
    int BurstMinShots = 2;
    int BurstMaxShots = 5;
    float TimeBetweenShotsMin = 0.3;
    float TimeBetweenShotsMax = 1.2;
    float ReactionTimeMin = 0.5;
    float ReactionTimeMax = 2.0;
}

class AloneZAnimationSettings
{
    int UseWalkAnimation = 1;
    int UseRunAnimation = 1;
    int UseSprintAnimation = 1;
    int UseCrouchAnimation = 1;
    int UseProneAnimation = 0;
    int UseAimAnimation = 1;
    int UseFireAnimation = 1;
    int UseReloadAnimation = 1;
    int UseDeathAnimation = 1;
    int UseIdleAnimation = 1;
    float TransitionSmoothing = 0.3;
}

class AloneZLogSettings
{
    int LogModStart = 1;
    int LogModStop = 1;
    int LogRouteStart = 1;
    int LogRouteEnd = 1;
    int LogBotSpawn = 1;
    int LogBotDeath = 1;
    int LogRouteIntercepted = 1;
    int LogCombatEngaged = 1;
    int LogCombatDisengaged = 1;
    int LogWaypointReached = 1;
    int LogConfigReload = 1;
    int LogToRPT = 0;
    int LogToFile = 1;
}

class AloneZGlobalSettings
{
    int MaxBotsTotal = 50;
    float BotDespawnDistance = 1000.0;
    int BotRespawnEnabled = 1;
    int BotRespawnDelaySeconds = 300;
    string BotDefaultSpeed = "WALK";
    string BotThreatSpeed = "SPRINT";
    float BotDetectionRange = 150.0;
    float BotEngageRange = 100.0;
    float BotDisengageRange = 200.0;
    int FriendlyFire = 0;
    int CanBeLooted = 1;
    string LootDropOnDeath = "ALL";
}

class AloneZBotAccuracyConfig
{
    float MinAccuracy = 0.3;
    float MaxAccuracy = 0.7;
    int OverrideGlobal = 0;
}

class AloneZBotHealthConfig
{
    float HealthMultiplier = 1.0;
    float DamageReceivedMultiplier = 1.0;
    float DamageDealtMultiplier = 1.0;
}

class AloneZBotBehaviorConfig
{
    int EngageOnSight = 1;
    int FleeOnLowHealth = 0;
    float FleeHealthThreshold = 20.0;
    int CallReinforcements = 0;
    int SearchAfterCombat = 1;
    float SearchDurationSeconds = 30;
    int ReturnToRouteAfterCombat = 1;
}

class AloneZBotAppearanceConfig
{
    ref TStringArray ClassNames = new TStringArray;
    int RandomizeAppearance = 1;
}

class AloneZWaypointConfig
{
    int Index = 0;
    string Name = "";
    vector Position = "0 0 0";
    float WaitTime = 5.0;
    string Speed = "WALK";
    string Stance = "ERECT";
    vector LookDirection = "0 0 1";
    string Action = "NONE";
}

class AloneZScheduleConfig
{
    int UseSchedule = 0;
    int ActiveHoursStart = 6;
    int ActiveHoursEnd = 22;
    ref TIntArray DaysOfWeek = new TIntArray;
}

class AloneZWeaponSlotConfig
{
    string ClassName = "";
    ref TStringArray Attachments = new TStringArray;
    string Magazine = "";
    int MagazineCount = 3;
}

class AloneZWeaponsConfig
{
    ref AloneZWeaponSlotConfig Primary = new AloneZWeaponSlotConfig;
    ref AloneZWeaponSlotConfig Secondary = new AloneZWeaponSlotConfig;
    string Melee = "";
}

class AloneZClothingConfig
{
    string Head = "";
    string Mask = "";
    string Top = "";
    string Vest = "";
    string Gloves = "";
    string Pants = "";
    string Shoes = "";
    string Back = "";
    string Belt = "";
}

class AloneZInventoryItemConfig
{
    string ClassName = "";
    int Quantity = 1;
}

class AloneZLoadoutConfig
{
    string LoadoutName = "";
    ref AloneZWeaponsConfig Weapons = new AloneZWeaponsConfig;
    ref AloneZClothingConfig Clothing = new AloneZClothingConfig;
    ref array<ref AloneZInventoryItemConfig> Inventory = new array<ref AloneZInventoryItemConfig>;
    int UnlimitedAmmo = 0;
}

class AloneZRouteConfig
{
    string RouteName = "";
    string RouteID = "";
    int Enabled = 1;
    int BotCount = 4;
    string Formation = "COLUMN";
    float FormationSpacing = 5.0;
    string Faction = "Military";
    string Loadout = "loadout_soldier";
    int RespawnOnComplete = 1;
    int LoopRoute = 1;
    string WaypointBehavior = "LOOP";
    string DefaultSpeed = "WALK";
    string DefaultStance = "ERECT";
    ref AloneZBotAccuracyConfig BotAccuracy = new AloneZBotAccuracyConfig;
    ref AloneZBotHealthConfig BotHealth = new AloneZBotHealthConfig;
    ref AloneZBotBehaviorConfig BotBehavior = new AloneZBotBehaviorConfig;
    ref AloneZBotAppearanceConfig BotAppearance = new AloneZBotAppearanceConfig;
    ref array<ref AloneZWaypointConfig> Waypoints = new array<ref AloneZWaypointConfig>;
    ref AloneZScheduleConfig Schedule = new AloneZScheduleConfig;
}

// Classe wrapper para serializar/deserializar o JSON de settings
class AloneZSettingsFile
{
    int ConfigVersion = 1;
    int ModEnabled = 1;
    int DebugMode = 0;
    int BotLogLevel = 2;
    ref AloneZGlobalSettings Global = new AloneZGlobalSettings;
    ref AloneZAccuracySettings Accuracy = new AloneZAccuracySettings;
    ref AloneZAnimationSettings Animation = new AloneZAnimationSettings;
    ref AloneZLogSettings Log = new AloneZLogSettings;
}

// --- Classe principal de configuracao ---

class AloneZBotsConfig
{
    static const string CONFIG_DIR = "$profile:AloneZ/Bots/Config/";
    static const string ROUTES_DIR = "$profile:AloneZ/Bots/Config/Routes/";
    static const string LOADOUTS_DIR = "$profile:AloneZ/Bots/Config/Loadouts/";
    static const string SETTINGS_FILE = "$profile:AloneZ/Bots/Config/AloneZBotsSettings.json";

    // Dados carregados
    static ref AloneZGlobalSettings m_GlobalSettings;
    static ref AloneZAccuracySettings m_AccuracySettings;
    static ref AloneZAnimationSettings m_AnimationSettings;
    static ref AloneZLogSettings m_LogSettings;
    static ref array<ref AloneZRouteConfig> m_Routes;
    static ref map<string, ref AloneZLoadoutConfig> m_Loadouts;

    static int m_ConfigVersion = 1;
    static int m_ModEnabled = 1;
    static int m_DebugMode = 0;
    static int m_BotLogLevel = 2;
    static bool m_IsLoaded = false;

    static void Load()
    {
        m_GlobalSettings = new AloneZGlobalSettings;
        m_AccuracySettings = new AloneZAccuracySettings;
        m_AnimationSettings = new AloneZAnimationSettings;
        m_LogSettings = new AloneZLogSettings;
        m_Routes = new array<ref AloneZRouteConfig>;
        m_Loadouts = new map<string, ref AloneZLoadoutConfig>;

        EnsureDirectories();

        if (FileExist(SETTINGS_FILE))
        {
            LoadSettings();
        }
        else
        {
            CreateDefaultSettings();
        }

        LoadLoadouts();
        LoadRoutes();

        m_IsLoaded = true;

        AloneZBotsLogger.SetDebugMode(m_DebugMode == 1);

        int numRoutes = m_Routes.Count();
        int numLoadouts = m_Loadouts.Count();
        Print("[AloneZ] Configuracoes carregadas. " + numRoutes.ToString() + " rotas, " + numLoadouts.ToString() + " loadouts.");
    }

    static void EnsureDirectories()
    {
        if (!FileExist("$profile:AloneZ"))
            MakeDirectory("$profile:AloneZ");
        if (!FileExist("$profile:AloneZ/Bots"))
            MakeDirectory("$profile:AloneZ/Bots");
        if (!FileExist(CONFIG_DIR))
            MakeDirectory(CONFIG_DIR);
        if (!FileExist(ROUTES_DIR))
            MakeDirectory(ROUTES_DIR);
        if (!FileExist(LOADOUTS_DIR))
            MakeDirectory(LOADOUTS_DIR);
        if (!FileExist("$profile:AloneZ/Bots/Logs"))
            MakeDirectory("$profile:AloneZ/Bots/Logs");
    }

    static void LoadSettings()
    {
        AloneZSettingsFile settings = new AloneZSettingsFile;
        JsonFileLoader<AloneZSettingsFile>.JsonLoadFile(SETTINGS_FILE, settings);

        m_ConfigVersion = settings.ConfigVersion;
        m_ModEnabled = settings.ModEnabled;
        m_DebugMode = settings.DebugMode;
        m_BotLogLevel = settings.BotLogLevel;

        if (settings.Global)
            m_GlobalSettings = settings.Global;
        if (settings.Accuracy)
            m_AccuracySettings = settings.Accuracy;
        if (settings.Animation)
            m_AnimationSettings = settings.Animation;
        if (settings.Log)
            m_LogSettings = settings.Log;
    }

    static void CreateDefaultSettings()
    {
        AloneZSettingsFile settings = new AloneZSettingsFile;
        JsonFileLoader<AloneZSettingsFile>.JsonSaveFile(SETTINGS_FILE, settings);

        CreateDefaultRoute();
        CreateDefaultLoadout();

        Print("[AloneZ] Arquivos de configuracao padrao criados em " + CONFIG_DIR);
    }

    static void CreateDefaultRoute()
    {
        AloneZRouteConfig route = new AloneZRouteConfig;
        route.RouteName = "Patrulha Base Militar";
        route.RouteID = "route_military_base";
        route.Enabled = 1;
        route.BotCount = 4;
        route.Formation = "COLUMN";
        route.FormationSpacing = 5.0;
        route.Faction = "Military";
        route.Loadout = "loadout_soldier";
        route.RespawnOnComplete = 1;
        route.LoopRoute = 1;
        route.WaypointBehavior = "LOOP";
        route.DefaultSpeed = "WALK";
        route.DefaultStance = "ERECT";

        route.BotAccuracy = new AloneZBotAccuracyConfig;
        route.BotAccuracy.MinAccuracy = 0.3;
        route.BotAccuracy.MaxAccuracy = 0.7;
        route.BotAccuracy.OverrideGlobal = 1;

        route.BotHealth = new AloneZBotHealthConfig;
        route.BotBehavior = new AloneZBotBehaviorConfig;

        route.BotAppearance = new AloneZBotAppearanceConfig;
        route.BotAppearance.ClassNames.Insert("SurvivorM_Mirek");
        route.BotAppearance.ClassNames.Insert("SurvivorM_Denis");
        route.BotAppearance.ClassNames.Insert("SurvivorM_Boris");
        route.BotAppearance.ClassNames.Insert("SurvivorM_Cyril");

        AloneZWaypointConfig wp0 = new AloneZWaypointConfig;
        wp0.Index = 0;
        wp0.Name = "Entrada Base";
        wp0.Position = "4523.50 340.20 8912.30";
        wp0.WaitTime = 5.0;
        wp0.Speed = "WALK";
        wp0.Stance = "ERECT";
        wp0.Action = "NONE";
        route.Waypoints.Insert(wp0);

        AloneZWaypointConfig wp1 = new AloneZWaypointConfig;
        wp1.Index = 1;
        wp1.Name = "Torre de Vigia";
        wp1.Position = "4550.80 342.10 8945.60";
        wp1.WaitTime = 10.0;
        wp1.Speed = "WALK";
        wp1.Stance = "CROUCH";
        wp1.Action = "SCAN";
        route.Waypoints.Insert(wp1);

        AloneZWaypointConfig wp2 = new AloneZWaypointConfig;
        wp2.Index = 2;
        wp2.Name = "Deposito de Armas";
        wp2.Position = "4580.20 339.80 8930.40";
        wp2.WaitTime = 8.0;
        wp2.Speed = "JOG";
        wp2.Stance = "ERECT";
        wp2.Action = "NONE";
        route.Waypoints.Insert(wp2);

        AloneZWaypointConfig wp3 = new AloneZWaypointConfig;
        wp3.Index = 3;
        wp3.Name = "Portao Sul";
        wp3.Position = "4510.00 338.50 8890.10";
        wp3.WaitTime = 5.0;
        wp3.Speed = "WALK";
        wp3.Stance = "ERECT";
        wp3.Action = "SCAN";
        route.Waypoints.Insert(wp3);

        route.Schedule = new AloneZScheduleConfig;

        string routePath = ROUTES_DIR + route.RouteID + ".json";
        JsonFileLoader<AloneZRouteConfig>.JsonSaveFile(routePath, route);
    }

    static void CreateDefaultLoadout()
    {
        AloneZLoadoutConfig loadout = new AloneZLoadoutConfig;
        loadout.LoadoutName = "loadout_soldier";

        loadout.Weapons = new AloneZWeaponsConfig;
        loadout.Weapons.Primary = new AloneZWeaponSlotConfig;
        loadout.Weapons.Primary.ClassName = "M4A1";
        loadout.Weapons.Primary.Attachments = new TStringArray;
        loadout.Weapons.Primary.Attachments.Insert("M4_RISHndgrd_Black");
        loadout.Weapons.Primary.Attachments.Insert("M4_OEBttstck_Black");
        loadout.Weapons.Primary.Magazine = "Mag_STANAG_30Rnd";
        loadout.Weapons.Primary.MagazineCount = 3;

        loadout.Weapons.Secondary = new AloneZWeaponSlotConfig;
        loadout.Weapons.Secondary.ClassName = "MakarovIJ70";
        loadout.Weapons.Secondary.Attachments = new TStringArray;
        loadout.Weapons.Secondary.Magazine = "Mag_IJ70_8Rnd";
        loadout.Weapons.Secondary.MagazineCount = 2;

        loadout.Weapons.Melee = "CombatKnife";

        loadout.Clothing = new AloneZClothingConfig;
        loadout.Clothing.Head = "MilitaryBeret_Red";
        loadout.Clothing.Top = "M65Jacket_Olive";
        loadout.Clothing.Vest = "PlateCarrierVest";
        loadout.Clothing.Gloves = "TacticalGloves_Black";
        loadout.Clothing.Pants = "CargoPants_Green";
        loadout.Clothing.Shoes = "MilitaryBoots_Black";
        loadout.Clothing.Back = "AssaultBag_Green";
        loadout.Clothing.Belt = "MilitaryBelt";

        loadout.Inventory = new array<ref AloneZInventoryItemConfig>;

        AloneZInventoryItemConfig item1 = new AloneZInventoryItemConfig;
        item1.ClassName = "BandageDressing";
        item1.Quantity = 2;
        loadout.Inventory.Insert(item1);

        AloneZInventoryItemConfig item2 = new AloneZInventoryItemConfig;
        item2.ClassName = "SalineBagIV";
        item2.Quantity = 1;
        loadout.Inventory.Insert(item2);

        loadout.UnlimitedAmmo = 0;

        string loadoutPath = LOADOUTS_DIR + loadout.LoadoutName + ".json";
        JsonFileLoader<AloneZLoadoutConfig>.JsonSaveFile(loadoutPath, loadout);

        // Cria loadout bandido
        AloneZLoadoutConfig bandit = new AloneZLoadoutConfig;
        bandit.LoadoutName = "loadout_bandit";
        bandit.Weapons = new AloneZWeaponsConfig;
        bandit.Weapons.Primary = new AloneZWeaponSlotConfig;
        bandit.Weapons.Primary.ClassName = "SKS";
        bandit.Weapons.Primary.Attachments = new TStringArray;
        bandit.Weapons.Primary.Magazine = "Mag_SKS_10Rnd";
        bandit.Weapons.Primary.MagazineCount = 2;
        bandit.Weapons.Secondary = new AloneZWeaponSlotConfig;
        bandit.Weapons.Melee = "KitchenKnife";
        bandit.Clothing = new AloneZClothingConfig;
        bandit.Clothing.Head = "BaseballCap_Black";
        bandit.Clothing.Top = "Hoodie_Black";
        bandit.Clothing.Pants = "Jeans_Black";
        bandit.Clothing.Shoes = "AthleticShoes_Black";
        bandit.Clothing.Back = "TaloonBag_Green";
        bandit.Inventory = new array<ref AloneZInventoryItemConfig>;
        bandit.UnlimitedAmmo = 0;

        string banditPath = LOADOUTS_DIR + bandit.LoadoutName + ".json";
        JsonFileLoader<AloneZLoadoutConfig>.JsonSaveFile(banditPath, bandit);

        // Cria loadout civil
        AloneZLoadoutConfig civilian = new AloneZLoadoutConfig;
        civilian.LoadoutName = "loadout_civilian";
        civilian.Weapons = new AloneZWeaponsConfig;
        civilian.Weapons.Primary = new AloneZWeaponSlotConfig;
        civilian.Weapons.Primary.ClassName = "Izh18";
        civilian.Weapons.Primary.Attachments = new TStringArray;
        civilian.Weapons.Primary.Magazine = "Ammo_762x39";
        civilian.Weapons.Primary.MagazineCount = 1;
        civilian.Weapons.Secondary = new AloneZWeaponSlotConfig;
        civilian.Weapons.Melee = "Hatchet";
        civilian.Clothing = new AloneZClothingConfig;
        civilian.Clothing.Top = "TShirt_White";
        civilian.Clothing.Pants = "Jeans_Blue";
        civilian.Clothing.Shoes = "HikingBoots_Brown";
        civilian.Clothing.Back = "DryBag_Blue";
        civilian.Inventory = new array<ref AloneZInventoryItemConfig>;
        civilian.UnlimitedAmmo = 0;

        string civPath = LOADOUTS_DIR + civilian.LoadoutName + ".json";
        JsonFileLoader<AloneZLoadoutConfig>.JsonSaveFile(civPath, civilian);
    }

    static void LoadRoutes()
    {
        string fileName;
        FileAttr fileAttr;
        FindFileHandle findHandle = FindFile(ROUTES_DIR + "*.json", fileName, fileAttr, 0);

        if (findHandle)
        {
            bool hasFile = true;
            while (hasFile)
            {
                if (fileName != "")
                {
                    string fullPath = ROUTES_DIR + fileName;
                    AloneZRouteConfig route = new AloneZRouteConfig;
                    JsonFileLoader<AloneZRouteConfig>.JsonLoadFile(fullPath, route);

                    if (route.RouteName != "" && route.Enabled)
                    {
                        m_Routes.Insert(route);
                        int wpCount = route.Waypoints.Count();
                        int botQty = route.BotCount;
                        Print("[AloneZ] Rota carregada: " + route.RouteName + " (" + route.RouteID + ") com " + wpCount.ToString() + " waypoints e " + botQty.ToString() + " bots.");
                    }
                }
                hasFile = FindNextFile(findHandle, fileName, fileAttr);
            }

            CloseFindFile(findHandle);
        }
    }

    static void LoadLoadouts()
    {
        string fileName;
        FileAttr fileAttr;
        FindFileHandle findHandle = FindFile(LOADOUTS_DIR + "*.json", fileName, fileAttr, 0);

        if (findHandle)
        {
            bool hasFile2 = true;
            while (hasFile2)
            {
                if (fileName != "")
                {
                    string fullPath = LOADOUTS_DIR + fileName;
                    AloneZLoadoutConfig loadout = new AloneZLoadoutConfig;
                    JsonFileLoader<AloneZLoadoutConfig>.JsonLoadFile(fullPath, loadout);

                    if (loadout.LoadoutName != "")
                    {
                        m_Loadouts.Set(loadout.LoadoutName, loadout);
                        Print("[AloneZ] Loadout carregado: " + loadout.LoadoutName);
                    }
                }
                hasFile2 = FindNextFile(findHandle, fileName, fileAttr);
            }

            CloseFindFile(findHandle);
        }
    }

    // --- Getters ---

    static bool IsLoaded()
    {
        return m_IsLoaded;
    }

    static bool IsEnabled()
    {
        return m_ModEnabled == 1;
    }

    static bool IsDebugMode()
    {
        return m_DebugMode == 1;
    }

    static AloneZGlobalSettings GetGlobalSettings()
    {
        return m_GlobalSettings;
    }

    static AloneZAccuracySettings GetAccuracySettings()
    {
        return m_AccuracySettings;
    }

    static AloneZAnimationSettings GetAnimationSettings()
    {
        return m_AnimationSettings;
    }

    static AloneZLogSettings GetLogSettings()
    {
        return m_LogSettings;
    }

    static array<ref AloneZRouteConfig> GetRoutes()
    {
        return m_Routes;
    }

    static AloneZLoadoutConfig GetLoadout(string name)
    {
        if (m_Loadouts && m_Loadouts.Contains(name))
            return m_Loadouts.Get(name);

        return null;
    }

    static int GetRouteCount()
    {
        if (m_Routes)
            return m_Routes.Count();
        return 0;
    }

    static int GetTotalBotCount()
    {
        int total = 0;
        if (m_Routes)
        {
            foreach (AloneZRouteConfig route : m_Routes)
            {
                total = total + route.BotCount;
            }
        }
        return total;
    }
}
