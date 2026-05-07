class CfgPatches
{
	class alonezbt
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters", "DZ_Weapons_Firearms", "DZ_Weapons_Melee", "DZ_Weapons_Ammunition", "DZ_Weapons_Projectiles", "DZ_Anims_Anm_Player", "DZ_Anims_Cfg", "DZ_Sounds_Effects", "DZ_Characters_Headgear", "DZ_Characters_Vests", "DZ_Data"};
	};
};

class CfgMods
{
	class alonezbt
	{
		dir = "alonezbt";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "AloneZ BOTS";
		credits = "AloneZ";
		author = "AloneZ";
		authorID = "";
		version = "1.0.0";
		extra = 0;
		type = "mod";
		
		dependencies[] = {"Game", "World", "Mission"};
		
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"alonezbt/Scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"alonezbt/Scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"alonezbt/Scripts/5_Mission"};
			};
		};
	};
};

// =====================================================
// CfgVehicles - Bot Survivors com Animation Graph custom
// =====================================================
class CfgVehicles
{
	// --- Base Survivors ---
	class SurvivorM_Mirek;
	class SurvivorM_Denis;
	class SurvivorM_Boris;
	class SurvivorM_Cyril;
	class SurvivorM_Elias;
	class SurvivorM_Francis;
	class SurvivorM_Guo;
	class SurvivorM_Hassan;
	class SurvivorM_Indar;
	class SurvivorM_Jose;
	class SurvivorM_Kaito;
	class SurvivorM_Lewis;
	class SurvivorM_Manua;
	class SurvivorM_Niki;
	class SurvivorM_Oliver;
	class SurvivorM_Peter;
	class SurvivorM_Quinn;
	class SurvivorM_Rolf;
	class SurvivorM_Seth;
	class SurvivorM_Taiki;
	class SurvivorF_Linda;
	class SurvivorF_Maria;
	class SurvivorF_Frida;
	class SurvivorF_Gabi;
	class SurvivorF_Helga;
	class SurvivorF_Irena;
	class SurvivorF_Judy;
	class SurvivorF_Keiko;
	class SurvivorF_Eva;
	class SurvivorF_Naomi;
	class SurvivorF_Baty;
	
	// --- eAI Survivors Males (com animation graph custom) ---
	class eAI_SurvivorM_Mirek: SurvivorM_Mirek
	{
		class enfAnimSys
		{
			class enfAnimSys
			{
				graphName = "alonezbt\Animations\player_main.agr";
			};
		};
	};
	class eAI_SurvivorM_Denis: SurvivorM_Denis
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Boris: SurvivorM_Boris
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Cyril: SurvivorM_Cyril
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Elias: SurvivorM_Elias
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Francis: SurvivorM_Francis
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Guo: SurvivorM_Guo
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Hassan: SurvivorM_Hassan
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Indar: SurvivorM_Indar
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Jose: SurvivorM_Jose
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Kaito: SurvivorM_Kaito
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Lewis: SurvivorM_Lewis
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Manua: SurvivorM_Manua
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Niki: SurvivorM_Niki
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Oliver: SurvivorM_Oliver
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Peter: SurvivorM_Peter
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Quinn: SurvivorM_Quinn
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Rolf: SurvivorM_Rolf
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Seth: SurvivorM_Seth
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorM_Taiki: SurvivorM_Taiki
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	
	// --- eAI Survivors Females (com animation graph custom) ---
	class eAI_SurvivorF_Linda: SurvivorF_Linda
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Maria: SurvivorF_Maria
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Frida: SurvivorF_Frida
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Gabi: SurvivorF_Gabi
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Helga: SurvivorF_Helga
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Irena: SurvivorF_Irena
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Judy: SurvivorF_Judy
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Keiko: SurvivorF_Keiko
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Eva: SurvivorF_Eva
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Naomi: SurvivorF_Naomi
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	class eAI_SurvivorF_Baty: SurvivorF_Baty
	{
		class enfAnimSys { class enfAnimSys { graphName = "alonezbt\Animations\player_main.agr"; }; };
	};
	
	// --- Gear customizado ---
	class GorkaHelmet;
	class Expansion_GorkaHelmet_Yeet: GorkaHelmet
	{
		scope = 2;
		visibilityModifier = 0.7;
		color = "Black";
		lootCategory = "Crafted";
		hiddenSelectionsTextures[] =
		{
			"alonezbt\Gear\Data\maska_yeet_co.paa",
			"alonezbt\Gear\Data\maska_yeet_co.paa",
			"alonezbt\Gear\Data\maska_yeet_co.paa"
		};
	};
	
	class PlateCarrierVest;
	class Expansion_PlateCarrierVest_Yeet: PlateCarrierVest
	{
		hiddenSelections[] = {"camoGround","camoMale","camoFemale"};
		hiddenSelectionsTextures[] =
		{
			"alonezbt\Gear\Data\BallisticVest_Yeet_co.paa",
			"alonezbt\Gear\Data\BallisticVest_Yeet_co.paa",
			"alonezbt\Gear\Data\BallisticVest_Yeet_co.paa"
		};
	};
	
	class PressVest_ColorBase;
	class Expansion_PressVest_Blue_Yeet: PressVest_ColorBase
	{
		scope = 2;
		visibilityModifier = 0.85;
		hiddenSelectionsTextures[] =
		{
			"alonezbt\Gear\Data\PressVest_Blue_Yeet_co.paa",
			"alonezbt\Gear\Data\PressVest_Blue_Yeet_co.paa",
			"alonezbt\Gear\Data\PressVest_Blue_Yeet_co.paa"
		};
		hiddenSelectionsMaterials[] =
		{
			"alonezbt\Gear\Data\pressvest_yeet.rvmat",
			"alonezbt\Gear\Data\pressvest_yeet.rvmat",
			"alonezbt\Gear\Data\pressvest_yeet.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] =
					{
						{1.0, {"alonezbt\Gear\Data\pressvest_yeet.rvmat"}},
						{0.7, {"alonezbt\Gear\Data\pressvest_yeet.rvmat"}},
						{0.5, {"alonezbt\Gear\Data\pressvest_yeet_damage.rvmat"}},
						{0.3, {"alonezbt\Gear\Data\pressvest_yeet_damage.rvmat"}},
						{0.0, {"alonezbt\Gear\Data\pressvest_yeet_destruct.rvmat"}}
					};
				};
			};
		};
	};
};

// =====================================================
// CfgSoundShaders - Sons do AI
// =====================================================
class CfgSoundShaders
{
	class baseCharacter_SoundShader;
	
	class Expansion_AI_The_Sound_Of_Love_01_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\The_Sound_Of_Love_01", 1}};
		volume = 1.0;
	};
	class Expansion_AI_The_Sound_Of_Love_02_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\The_Sound_Of_Love_02", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_0_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\o", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_1_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\1", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_2_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\2", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_3_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\3", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_4_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\4", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_5_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\5", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_6_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\6", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_7_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\7", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_8_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\8", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_9_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\9", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_i_can_see_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\i_can_see", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_hundred_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\hundred", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_jack_shit_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\jack_shit", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_meters_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\meters", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Female_thousand_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\thousand", 1}};
		volume = 1.0;
	};
	class Expansion_AI_ShoryukenF_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\ShoryukenF", 1}};
		volume = 1.0;
	};
	class Expansion_AI_ShoryukenM_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\ShoryukenM", 1}};
		volume = 1.0;
	};
	class Expansion_AI_Heavy_Punch_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\Heavy_Punch", 1}};
		volume = 1.0;
	};
	class Expansion_AI_UahUahUahM_SoundShader: baseCharacter_SoundShader
	{
		samples[] = {{"alonezbt\Sounds\UahUahUahM", 1}};
		volume = 1.0;
	};
};

// =====================================================
// CfgSoundSets - SoundSets do AI
// =====================================================
class CfgSoundSets
{
	class baseCharacter_SoundSet;
	
	class Expansion_AI_The_Sound_Of_Love_01_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_The_Sound_Of_Love_01_SoundShader"};
	};
	class Expansion_AI_The_Sound_Of_Love_02_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_The_Sound_Of_Love_02_SoundShader"};
	};
	class Expansion_AI_Female_0_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_0_SoundShader"};
	};
	class Expansion_AI_Female_1_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_1_SoundShader"};
	};
	class Expansion_AI_Female_2_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_2_SoundShader"};
	};
	class Expansion_AI_Female_3_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_3_SoundShader"};
	};
	class Expansion_AI_Female_4_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_4_SoundShader"};
	};
	class Expansion_AI_Female_5_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_5_SoundShader"};
	};
	class Expansion_AI_Female_6_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_6_SoundShader"};
	};
	class Expansion_AI_Female_7_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_7_SoundShader"};
	};
	class Expansion_AI_Female_8_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_8_SoundShader"};
	};
	class Expansion_AI_Female_9_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_9_SoundShader"};
	};
	class Expansion_AI_Female_i_can_see_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_i_can_see_SoundShader"};
	};
	class Expansion_AI_Female_hundred_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_hundred_SoundShader"};
	};
	class Expansion_AI_Female_jack_shit_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_jack_shit_SoundShader"};
	};
	class Expansion_AI_Female_meters_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_meters_SoundShader"};
	};
	class Expansion_AI_Female_thousand_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Female_thousand_SoundShader"};
	};
	class Expansion_AI_ShoryukenF_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_ShoryukenF_SoundShader"};
	};
	class Expansion_AI_ShoryukenM_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_ShoryukenM_SoundShader"};
	};
	class Expansion_AI_Heavy_Punch_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_Heavy_Punch_SoundShader"};
	};
	class Expansion_AI_UahUahUahM_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = {"Expansion_AI_UahUahUahM_SoundShader"};
	};
};

// =====================================================
// CfgAmmo - Ammo Debug
// =====================================================
class CfgAmmo
{
	class Bullet_762x39Tracer;
	class Bullet_762x39Tracer_eAI_Debug: Bullet_762x39Tracer
	{
		DamageApplied
		{
			type = "Projectile";
			dispersion = 0;
			bleedThreshold = 0;
			class Health { damage = 0; };
			class Blood { damage = 0; };
			class Shock { damage = 0; };
		};
	};
	
	class MeleeFist_Heavy;
	class MeleeShoryuken: MeleeFist_Heavy
	{
	};
};

class CfgAmmoTypes
{
	class AType_Bullet_762x39Tracer_eAI_Debug
	{
		name = "Bullet_762x39Tracer_eAI_Debug";
	};
};

// =====================================================
// CfgMagazines - Magazine Debug
// =====================================================
class CfgMagazines
{
	class Mag_AKM_Drum75Rnd;
	class Mag_AKM_Drum_eAI_Debug: Mag_AKM_Drum75Rnd
	{
		scope = 2;
		displayName = "AloneZ AI Debug AKM Mag";
		descriptionShort = "Uses special ammo that does no health damage.";
		ammo = "Bullet_762x39Tracer_eAI_Debug";
	};
};

// =====================================================
// CfgWeapons - Ammo Debug
// =====================================================
class CfgWeapons
{
	class Ammo_762x39Tracer;
	class Ammo_762x39Tracer_eAI_Debug: Ammo_762x39Tracer
	{
		scope = 2;
		displayName = "AloneZ AI Debug AKM Tracer Ammo";
		descriptionShort = "Special ammo that does no health damage.";
		ammo = "Bullet_762x39Tracer_eAI_Debug";
	};
};
