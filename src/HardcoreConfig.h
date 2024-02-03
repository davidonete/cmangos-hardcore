#pragma once

#include "Config/Config.h"

class HardcoreConfig
{
public:
    HardcoreConfig();

    static HardcoreConfig& instance()
    {
        static HardcoreConfig instance;
        return instance;
    }

    bool Initialize();

public:
    bool enabled;
    bool spawnGrave;
    uint32 graveGameObjectId;
    std::string graveMessage;
    float dropGearPct;
    float dropItemsPct;
    float dropMoneyPct;
#ifdef ENABLE_MANGOSBOTS
    float botDropGearPct;
    float botDropItemsPct;
    float botDropMoneyPct;
#endif
    uint32 lootGameObjectId;
    bool reviveDisabled;
    bool reviveOnGraveyard;
    float levelDownPct;
    uint32 maxDroppedLoot;

private:
    Config config;
};

#define sHardcoreConfig MaNGOS::Singleton<HardcoreConfig>::Instance()

