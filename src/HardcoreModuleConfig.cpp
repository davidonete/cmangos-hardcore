#include "HardcoreModuleConfig.h"
#include "Server/DBCEnums.h"

namespace cmangos_module
{
    HardcoreModuleConfig::HardcoreModuleConfig()
    : ModuleConfig("hardcore.conf")
    , enabled(false)
    , spawnGrave(false)
    , graveGameObjectId(0U)
    , graveMessage("")
    , dropGearPct(0.0f)
    , dropItemsPct(0.0f)
    , dropMoneyPct(0.0f)
    #ifdef ENABLE_PLAYERBOTS
    , botDropGearPct(0.0f)
    , botDropItemsPct(0.0f)
    , botDropMoneyPct(0.0f)
    #endif
    , lootGameObjectId(0U)
    , reviveDisabled(false)
    , reviveOnGraveyard(false)
    , levelDownPct(0.0f)
    , maxDroppedLoot(0U)
    , dropOnDungeons(false)
    , dropOnRaids(false)
    , levelDownOnDungeons(false)
    , levelDownOnRaids(false)
    , dropMinLevel(0)
    , dropMaxLevel(0)
    , levelDownMinLevel(0)
    , levelDownMaxLevel(0)
    {

    }

    bool HardcoreModuleConfig::OnLoad()
    {
        enabled = config.GetBoolDefault("Hardcore.Enable", false);
        spawnGrave = config.GetBoolDefault("Hardcore.SpawnGrave", false);
        graveGameObjectId = config.GetIntDefault("Hardcore.GraveGameObjectID", 0U);
        graveMessage = config.GetStringDefault("Hardcore.GraveMessage", "Here lies <PlayerName>");
        dropGearPct = config.GetFloatDefault("Hardcore.DropGear", 0.0f);
        dropItemsPct = config.GetFloatDefault("Hardcore.DropItems", 0.0f);
        dropMoneyPct = config.GetFloatDefault("Hardcore.DropMoney", 0.0f);
    #ifdef ENABLE_PLAYERBOTS
        botDropGearPct = config.GetFloatDefault("Hardcore.BotDropGear", 0.0f);
        botDropItemsPct = config.GetFloatDefault("Hardcore.BotDropItems", 0.0f);
        botDropMoneyPct = config.GetFloatDefault("Hardcore.BotDropMoney", 0.0f);
    #endif
        lootGameObjectId = config.GetIntDefault("Hardcore.LootGameObjectID", 0U);
        reviveDisabled = config.GetBoolDefault("Hardcore.ReviveDisabled", false);
        reviveOnGraveyard = config.GetBoolDefault("Hardcore.ReviveOnGraveyard", false);
        levelDownPct = config.GetFloatDefault("Hardcore.LevelDown", 0.0f);
        maxDroppedLoot = config.GetIntDefault("Hardcore.MaxPlayerLoot", 0U);
        dropOnDungeons = config.GetBoolDefault("Hardcore.DropOnDungeons", false);
        dropOnRaids = config.GetBoolDefault("Hardcore.DropOnRaids", false);
        levelDownOnDungeons = config.GetBoolDefault("Hardcore.LevelDownOnDungeons", false);
        levelDownOnRaids = config.GetBoolDefault("Hardcore.LevelDownOnRaids", false);
        dropMinLevel = config.GetIntDefault("Hardcore.DropMinLevel", 1);
        dropMaxLevel = config.GetIntDefault("Hardcore.DropMaxLevel", DEFAULT_MAX_LEVEL);
        levelDownMinLevel = config.GetIntDefault("Hardcore.LevelDownMinLevel", 1);
        levelDownMaxLevel = config.GetIntDefault("Hardcore.LevelDownMaxLevel", DEFAULT_MAX_LEVEL);
        return true;
    }
}