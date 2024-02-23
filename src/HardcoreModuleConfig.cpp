#include "HardcoreConfig.h"

#include "Log.h"
#include "SystemConfig.h"

HardcoreConfig::HardcoreConfig()
: enabled(false)
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
{

}

INSTANTIATE_SINGLETON_1(HardcoreConfig);

bool HardcoreConfig::Initialize()
{
    sLog.outString("Initializing Hardcore by Flekz");

    if (!config.SetSource(SYSCONFDIR"hardcore.conf"))
    {
        sLog.outError("Failed to open configuration file hardcore.conf");
        return false;
    }

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

    sLog.outString("Hardcore configuration loaded");
    return true;
}
