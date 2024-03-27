#pragma once
#include "ModuleConfig.h"

namespace cmangos_module
{
    class HardcoreModuleConfig : public ModuleConfig
    {
    public:
        HardcoreModuleConfig();
        bool OnLoad() override;

    public:
        bool enabled;
        bool spawnGrave;
        uint32 graveGameObjectId;
        std::string graveMessage;
        float dropGearPct;
        float dropItemsPct;
        float dropMoneyPct;
    #ifdef ENABLE_PLAYERBOTS
        float botDropGearPct;
        float botDropItemsPct;
        float botDropMoneyPct;
    #endif
        uint32 lootGameObjectId;
        bool reviveDisabled;
        bool reviveOnGraveyard;
        float levelDownPct;
        uint32 maxDroppedLoot;
        bool dropOnDungeons;
        bool dropOnRaids;
        bool levelDownOnDungeons;
        bool levelDownOnRaids;
        uint32 dropMinLevel;
        uint32 dropMaxLevel;
        uint32 levelDownMinLevel;
        uint32 levelDownMaxLevel;
    };
}