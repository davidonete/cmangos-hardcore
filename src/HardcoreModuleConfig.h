#pragma once
#include "ModuleConfig.h"

namespace cmangos_module
{
    #define HARDCORE_NPC_ENTRY 190011

    #define HARDCORE_SPELL_HARDCORE_CHALLENGE 33500
    #define HARDCORE_SPELL_SELF_FOUND_CHALLENGE 33501

    enum HardcoreDialogueMessage
    {
        HARDCORE_DIALOGUE_MESSAGE_MAIN = 50900,
        HARDCORE_DIALOGUE_MESSAGE_MAIN_DISABLED,
        HARDCORE_DIALOGUE_MESSAGE_HARDCORE_CHALLENGE,
        HARDCORE_DIALOGUE_MESSAGE_DROP_LOOT_CHALLENGE,
        HARDCORE_DIALOGUE_MESSAGE_LOSE_XP_CHALLENGE,
        HARDCORE_DIALOGUE_MESSAGE_SELF_FOUND_CHALLENGE,
        HARDCORE_DIALOGUE_MESSAGE_CANT_TAKE_CHALLENGE,
        HARDCORE_DIALOGUE_MESSAGE_ALREADY_TAKEN_CHALLENGE,
        HARDCORE_DIALOGUE_MESSAGE_ACCEPT_CHALLENGE,
        HARDCORE_DIALOGUE_MESSAGE_DISABLE_PVP,
        HARDCORE_DIALOGUE_MESSAGE_DISABLE_PVP_CONFIRM,
    };

    enum HardcoreDialogueOptions
    {
        HARDCORE_DIALOGUE_OPTION_HARDCORE_CHALLENGE = 12200,
        HARDCORE_DIALOGUE_OPTION_DROP_LOOT_CHALLENGE,
        HARDCORE_DIALOGUE_OPTION_LOSE_XP_CHALLENGE,
        HARDCORE_DIALOGUE_OPTION_SELF_FOUND_CHALLENGE,
        HARDCORE_DIALOGUE_OPTION_ACCEPT_CHALLENGE,
        HARDCORE_DIALOGUE_OPTION_DECLINE_CHALLENGE,
        HARDCORE_DIALOGUE_OPTION_DISABLE_PVP,
        HARDCORE_DIALOGUE_OPTION_ACCEPT,
        HARDCORE_DIALOGUE_OPTION_DECLINE
    };

    enum HardcoreDeathReason
    {
        HARDCORE_DEATH_REASON_UNKNOWN = 0,
        HARDCORE_DEATH_REASON_NPC_KILL,
        HARDCORE_DEATH_REASON_PLAYER_KILL,
        HARDCORE_DEATH_REASON_EXHAUSTED,
        HARDCORE_DEATH_REASON_DROWNING,
        HARDCORE_DEATH_REASON_FALL,
        HARDCORE_DEATH_REASON_LAVA,
        HARDCORE_DEATH_REASON_SLIME,
        HARDCORE_DEATH_REASON_FIRE,
        HARDCORE_DEATH_REASON_FALL_TO_VOID
    };

    enum HardcoreDeathFilter
    {
        HARDCORE_DEATH_FILTER_PLAYER = 0,
        HARDCORE_DEATH_FILTER_ACCOUNT,
        HARDCORE_DEATH_FILTER_WORLD
    };

    class HardcoreModuleConfig : public ModuleConfig
    {
    public:
        HardcoreModuleConfig();
        bool OnLoad() override;

        bool IsDropLootEnabled() const;

    public:
        bool enabled;
        bool playerConfig;
        bool broadcastDeathGuild;
        bool broadcastDeathWorld;
        bool spawnGrave;
        uint32 graveGameObjectId;
        std::string graveMessage;
        bool removeGraveOnCharacterDeleted;
        float dropGearPct;
        float dropItemsPct;
        float dropMoneyPct;
#ifdef ENABLE_PLAYERBOTS
        float botDropGearPct;
        float botDropItemsPct;
        float botDropMoneyPct;
#endif
        bool removeLootOnCharacterDeleted;
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
        bool disablePVP;
        bool selfFound;
    };
}