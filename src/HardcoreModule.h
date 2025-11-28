#ifndef CMANGOS_MODULE_HARDCORE_H
#define CMANGOS_MODULE_HARDCORE_H

#include "Module.h"
#include "HardcoreModuleConfig.h"

#include "Entities/ObjectGuid.h"

#include <utility>
#include <vector>
#include <string>
#include <map>

namespace cmangos_module
{
    class HardcoreModule;

    // Bag, Slot
    typedef std::pair<uint8, uint8> ItemSlot;

    struct HardcoreLootItem
    {
        HardcoreLootItem(uint32 id, uint8 amount);
        HardcoreLootItem(uint32 id, uint8 amount, const std::vector<ItemSlot>& slots);
        HardcoreLootItem(uint32 id, uint8 amount, uint32 randomPropertyId, uint32 durability, const std::string& enchantments);
        HardcoreLootItem(uint32 id, uint8 amount, uint32 randomPropertyId, uint32 durability, const std::string& enchantments, const std::vector<ItemSlot>& slots);

        uint32 m_id;
        bool m_isGear;
        uint32 m_randomPropertyId;
        uint32 m_durability;
        std::string m_enchantments;
        uint8 m_amount;
        std::vector<ItemSlot> m_slots;
    };

    class HardcoreLootGameObject
    {
    private:
        HardcoreLootGameObject(uint32 id, uint32 playerId, uint32 lootId, uint32 lootTableId, uint32 money, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const std::vector<HardcoreLootItem>& items, const HardcoreModuleConfig* moduleConfig);

    public:
        static HardcoreLootGameObject Load(uint32 id, uint32 playerId, const HardcoreModuleConfig* moduleConfig);
        static HardcoreLootGameObject Create(uint32 playerId, uint32 lootId, uint32 money, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const std::vector<HardcoreLootItem>& items, const HardcoreModuleConfig* moduleConfig);

        void Spawn();
        void DeSpawn();
        bool IsSpawned() const;
        void Destroy();

        uint32 GetId() const { return m_id; }
        uint32 GetGUID() const { return m_guid; }
        uint32 GetPlayerId() const { return m_playerId; }
        uint32 GetLootId() const { return m_lootId; }
        uint32 GetMoney() const { return m_money; }
        void SetMoney(uint32 money);
        bool HasItems() const { return !m_items.empty(); }
        const std::vector<HardcoreLootItem>& GetItems() const { return m_items; }
        const HardcoreLootItem* GetItem(uint32 itemId) const;
        bool RemoveItem(uint32 itemId);

    private:
        uint32 m_id;
        uint32 m_playerId;
        uint32 m_guid;
        uint32 m_lootId;
        uint32 m_lootTableId;
        uint32 m_money;
        float m_positionX;
        float m_positionY;
        float m_positionZ;
        float m_orientation;
        uint32 m_mapId;
        uint32 m_phaseMask;
        std::vector<HardcoreLootItem> m_items;
        const HardcoreModuleConfig* m_moduleConfig;
    };

    class HardcorePlayerLoot
    {
    public:
        HardcorePlayerLoot(uint32 id, uint32 playerId, HardcoreModule* module);
        void LoadGameObject(uint32 gameObjectId);
        HardcoreLootGameObject* FindGameObjectByGUID(const uint32 guid);
        bool RemoveGameObject(uint32 gameObjectId);

        bool HasGameObjects() const { return !m_gameObjects.empty(); }
        uint32 GetPlayerId() const { return m_playerId; }
        uint32 GetId() const { return m_id; }

        void Spawn();
        void DeSpawn();
        bool Create();
        void Destroy();

    private:
        uint32 m_id; 
        uint32 m_playerId;
        std::vector<HardcoreLootGameObject> m_gameObjects;
        HardcoreModule* m_module;
    };

    class HardcoreGraveGameObject
    {
    private:
        HardcoreGraveGameObject(uint32 id, uint32 gameObjectEntry, uint32 playerId, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const HardcoreModuleConfig* moduleConfig);

    public:
        static HardcoreGraveGameObject Load(uint32 id, const HardcoreModuleConfig* moduleConfig);
        static HardcoreGraveGameObject Create(uint32 playerId, uint32 gameObjectEntry, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const HardcoreModuleConfig* moduleConfig);

        void Spawn();
        void DeSpawn();
        bool IsSpawned() const;
        void Destroy();

    private:
        uint32 m_id;
        uint32 m_gameObjectEntry;
        uint32 m_guid;
        uint32 m_playerId;
        float m_positionX;
        float m_positionY;
        float m_positionZ;
        float m_orientation;
        uint32 m_mapId;
        uint32 m_phaseMask;
        const HardcoreModuleConfig* m_moduleConfig;
    };

    class HardcorePlayerGrave
    {
    private:
        HardcorePlayerGrave(uint32 playerId, uint32 gameObjectEntry, const HardcoreModuleConfig* moduleConfig);
        HardcorePlayerGrave(uint32 playerId, uint32 gameObjectEntry, const std::vector<HardcoreGraveGameObject>& gameObjects, const HardcoreModuleConfig* moduleConfig);

    public:
        static HardcorePlayerGrave Load(uint32 playerId, uint32 gameObjectEntry, const HardcoreModuleConfig* moduleConfig);
        static HardcorePlayerGrave Generate(uint32 playerId, const std::string& playerName, const HardcoreModuleConfig* moduleConfig);

        void Spawn();
        void DeSpawn();
        void Create();
        void Destroy();

    private:
        static std::string GenerateGraveMessage(const std::string& playerName, const HardcoreModuleConfig* moduleConfig);

    private:
        uint32 m_playerId;
        uint32 m_gameObjectEntry;
        std::vector<HardcoreGraveGameObject> m_gameObjects;
        const HardcoreModuleConfig* m_moduleConfig;
    };

    class HardcorePlayerConfig
    {
    private:
        HardcorePlayerConfig(uint32 playerId);

    public:
        static HardcorePlayerConfig Load(uint32 playerId);
        void Destroy();

        bool IsReviveDisabled() const { return m_reviveDisabled; }
        bool ShouldDropLootOnDeath() const { return m_dropLootOnDeath; }
        bool ShouldLoseXPOnDeath() const { return m_loseXPOnDeath; }
        bool IsPVPDisabled() const { return m_pvpDisabled; }
        bool IsSelfFound() const { return m_selfFound; }
        float GetXPRate() const { return m_xpRate / 100.0f; }

        void ToggleReviveDisabled(bool enable);
        void ToggleDropLootOnDeath(bool enable);
        void ToggleLoseXPOnDeath(bool enable);
        void TogglePVPDisabled(bool enable);
        void ToggleSelfFound(bool enable);
        void SetXPRate(float rate);

        static bool HasSameChallenges(const HardcorePlayerConfig* playerConfig, const HardcorePlayerConfig* otherPlayerConfig);

        Player* GetPlayer() const;
        const Player* GetPlayerConst() const;

    private:
        void ToggleAura(bool enable, uint32 spellId);

    private:
        uint32 m_playerId;

        bool m_reviveDisabled;
        bool m_dropLootOnDeath;
        bool m_loseXPOnDeath;
        bool m_pvpDisabled;
        bool m_selfFound;
        uint32 m_xpRate;
    };

    class HardcorePlayerDeathLogEntry
    {
    public:
        HardcorePlayerDeathLogEntry(uint32 playerId, uint32 accountId, const std::string& playerName, uint32 level, uint32 zoneId, uint32 areaId, uint32 mapId, uint32 killerId, const std::string& killerName, HardcoreDeathReason reason, time_t date);

        std::string GetDateTime() const;
        uint32 GetAccountId() const { return m_accountId; }
        const std::string& GetPlayerName() const { return m_playerName; }
        uint32 GetLevel() const { return m_level; }
        std::string GetZoneName(const Player* player) const;
        std::string GetAreaName(const Player* player) const;
        std::string GetMapName(const Player* player) const;
        uint32 GetKillerId() const { return m_killerId; }
        const std::string& GetKillerName() const { return m_killerName; }
        std::string GetNPCKillerName(const Player* player) const;
        HardcoreDeathReason GetReason() const { return m_reason; }
        time_t GetDate() const { return m_date; }

        std::string GetMessage(const Player* player) const;

    private:
        uint32 m_playerId;
        uint32 m_accountId;
        std::string m_playerName;
        uint32 m_level;
        uint32 m_zoneId;
        uint32 m_areaId;
        uint32 m_mapId;
        uint32 m_killerId;
        std::string m_killerName;
        HardcoreDeathReason m_reason;
        time_t m_date;
    };

    class HardcorePlayerDeathLog
    {
    public:
        HardcorePlayerDeathLog() {}

        void Load();

        void OnDeath(Player* player, const HardcoreModuleConfig* moduleConfig, const Unit* killer = nullptr, int8 environmentDamageType = -1);

        std::vector<const HardcorePlayerDeathLogEntry*> GetEntries(HardcoreDeathFilter filter, uint8 amount, uint32 accountId = 0, std::string playerName = "") const;

    private:
        void Add(uint32 playerId, uint32 accountId, const std::string& playerName, uint32 level, uint32 zoneId, uint32 areaId, uint32 mapId, uint32 killerId, const std::string& killerName, HardcoreDeathReason reason, time_t date);

    private:
        std::vector<HardcorePlayerDeathLogEntry> entries;
    };

    class HardcoreModule : public Module
    {
        friend HardcorePlayerLoot;

    public:
        HardcoreModule();
        const HardcoreModuleConfig* GetConfig() const override;

        void OnWorldPreInitialized() override;
        void OnInitialize() override;

        // Player hooks
        void OnCharacterCreated(Player* player) override;
        void OnDeleteFromDB(uint32 playerId) override;
        bool OnPreResurrect(Player* player) override;
        void OnResurrect(Player* player) override;
        void OnDeath(Player* player, Unit* killer) override;
        void OnDeath(Player* player, uint8 environmentalDamageType) override;
        void OnReleaseSpirit(Player* player, const WorldSafeLocsEntry* closestGrave) override;
        void OnStoreItem(Player* player, Loot* loot, Item* item) override;
        bool OnPreHandleInitializeTrade(Player* player, Player* trader) override;
        bool OnPreGiveXP(Player* player, uint32& xp, Creature* victim) override;

        // Unit hooks
        bool OnGetReactionTo(const Unit* unit, const Unit* target, ReputationRank& outReaction) override;

        // Game object hooks
        bool OnCanCheckMailBox(Player* player, const ObjectGuid& mailboxGuid, bool& outResult) override;

        // Loot hooks
        bool OnFillLoot(Loot* loot, Player* owner) override;
        bool OnGenerateMoneyLoot(Loot* loot, uint32& outMoney) override;
        void OnAddItem(Loot* loot, LootItem* lootItem) override;
        void OnSendGold(Loot* loot, Player* player, uint32 gold, uint8 lootMethod) override;

        // Group hooks
        bool OnPreInviteMember(Group* group, Player* player, Player* recipient) override;

        // Gossip hooks
        bool OnPreGossipHello(Player* player, Creature* creature) override;
        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action, const std::string& code, uint32 gossipListId) override;

        // Commands
        std::vector<ModuleChatCommand>* GetCommandTable() override;
        const char* GetChatCommandPrefix() const override { return "hardcore"; }
        bool HandleResetCommand(WorldSession* session, const std::string& args);
        bool HandleResetGravesCommand(WorldSession* session, const std::string& args);
        bool HandleResetLootCommand(WorldSession* session, const std::string& args);
        bool HandleSpawnLootCommand(WorldSession* session, const std::string& args);
        bool HandleSpawnGraveCommand(WorldSession* session, const std::string& args);
        bool HandleLevelDownCommand(WorldSession* session, const std::string& args);
        bool HandleToggleReviveCommand(WorldSession* session, const std::string& args);
        bool HandleToggleDropLootCommand(WorldSession* session, const std::string& args);
        bool HandleToggleLoseXPCommand(WorldSession* session, const std::string& args);
        bool HandleTogglePVPCommand(WorldSession* session, const std::string& args);
        bool HandleToggleSelfFoundCommand(WorldSession* session, const std::string& args);
        bool HandleDeathlogCommand(WorldSession* session, const std::string& args);
        bool HandleXPRateCommand(WorldSession* session, const std::string& args);

        HardcorePlayerConfig* GetPlayerConfig(uint32 playerId);
        HardcorePlayerConfig* GetPlayerConfig(const Player* player);

    private:
        // Loot methods
        HardcoreLootGameObject* FindLootGOByGUID(const uint32 guid);
        HardcorePlayerLoot* FindLootByID(const uint32 playerId, const uint32 lootId);

        void PreLoadLoot();
        void LoadLoot();
        void RemoveAllLoot();
        void CreateLoot(Player* player, Unit* killer);
        bool RemoveLoot(uint32 playerId, uint32 lootId);

        // Grave methods
        void RemoveAllGraves();
        void CreateGrave(Player* player, Unit* killer = nullptr);
        void GenerateMissingGraves();
        void PreLoadGraves();
        void LoadGraves();

        // Level methods
        void LevelDown(Player* player, Unit* killer = nullptr);

    public:
        Unit* GetKiller(Player* player) const;
        void  SetKiller(Player* player, Unit* killer);

    private:
        std::map<uint32, ObjectGuid> m_lastPlayerDeaths;
        std::unordered_map<uint32, HardcorePlayerGrave> m_playerGraves;
        std::unordered_map<uint32, std::map<uint32, HardcorePlayerLoot>> m_playersLoot;
        std::unordered_map<uint32, HardcorePlayerConfig> m_playerManagers;
        HardcorePlayerDeathLog m_deathLog;
        bool m_getReactionToInternal;
    };
}
#endif