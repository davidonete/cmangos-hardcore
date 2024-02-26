#ifndef MANGOS_HARDCORE_MODULE_H
#define MANGOS_HARDCORE_MODULE_H

#include "Module.h"
#include "HardcoreModuleConfig.h"

#include "Entities/ObjectGuid.h"

#include <utility>
#include <vector>
#include <string>
#include <map>

namespace hardcore_module
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
        HardcoreLootGameObject(uint32 id, uint32 playerId, uint32 lootId, uint32 lootTableId, uint32 money, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const std::vector<HardcoreLootItem>& items, HardcoreModule* module);

    public:
        static HardcoreLootGameObject Load(uint32 id, uint32 playerId, HardcoreModule* module);
        static HardcoreLootGameObject Create(uint32 playerId, uint32 lootId, uint32 money, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const std::vector<HardcoreLootItem>& items, HardcoreModule* module);

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
        HardcoreModule* m_module;
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
        HardcoreGraveGameObject(uint32 id, uint32 gameObjectEntry, uint32 playerId, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, HardcoreModule* module);

    public:
        static HardcoreGraveGameObject Load(uint32 id, HardcoreModule* module);
        static HardcoreGraveGameObject Create(uint32 playerId, uint32 gameObjectEntry, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, HardcoreModule* module);

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
        HardcoreModule* m_module;
    };

    class HardcorePlayerGrave
    {
    private:
        HardcorePlayerGrave(uint32 playerId, uint32 gameObjectEntry, HardcoreModule* module);
        HardcorePlayerGrave(uint32 playerId, uint32 gameObjectEntry, const std::vector<HardcoreGraveGameObject>& gameObjects, HardcoreModule* module);

    public:
        static HardcorePlayerGrave Load(uint32 playerId, uint32 gameObjectEntry, HardcoreModule* module);
        static HardcorePlayerGrave Generate(uint32 playerId, const std::string& playerName, HardcoreModule* module);

        void Spawn();
        void DeSpawn();
        void Create();
        void Destroy();

    private:
        static std::string GenerateGraveMessage(const std::string& playerName, HardcoreModule* module);

    private:
        uint32 m_playerId;
        uint32 m_gameObjectEntry;
        std::vector<HardcoreGraveGameObject> m_gameObjects;
        HardcoreModule* m_module;
    };

    class HardcoreModule : public Module
    {
        friend HardcorePlayerLoot;

    public:
        HardcoreModule() : Module("Hardcore") {}
        HardcoreModuleConfig* CreateConfig() override { return new HardcoreModuleConfig(); }
        const HardcoreModuleConfig* GetConfig() const override { return (HardcoreModuleConfig*)GetConfigInternal(); }

        void OnWorldPreInitialized() override;
        void OnInitialize() override;

        // Player hooks
        void OnCharacterCreated(Player* player) override;
        void OnDeleteFromDB(uint32 playerId) override;
        bool OnPreResurrect(Player* player) override;
        void OnResurrect(Player* player) override;
        void OnDeath(Player* player, Unit* killer) override;
        void OnReleaseSpirit(Player* player, const WorldSafeLocsEntry* closestGrave) override;
        void OnStoreNewItem(Player* player, Loot* loot, Item* item) override;

        // Loot hooks
        bool OnFillLoot(Loot* loot, Player* owner) override;
        bool OnGenerateMoneyLoot(Loot* loot, uint32& outMoney) override;
        void OnAddItem(Loot* loot, LootItem* lootItem) override;
        void OnSendGold(Loot* loot, uint32 gold) override;

        // Commands
        std::vector<ModuleChatCommand>* GetCommandTable() override;
        const char* GetChatCommandPrefix() const override { return "hardcore"; }
        bool HandleResetCommand(WorldSession* session, const std::string& args);
        bool HandleResetGravesCommand(WorldSession* session, const std::string& args);
        bool HandleResetLootCommand(WorldSession* session, const std::string& args);
        bool HandleSpawnLootCommand(WorldSession* session, const std::string& args);
        bool HandleSpawnGraveCommand(WorldSession* session, const std::string& args);
        bool HandleLevelDownCommand(WorldSession* session, const std::string& args);

    private:
        // Loot methods
        HardcoreLootGameObject* FindLootGOByGUID(const uint32 guid);
        HardcorePlayerLoot* FindLootByID(const uint32 playerId, const uint32 lootId);

        void PreLoadLoot();
        void LoadLoot();
        void RemoveAllLoot();
        void CreateLoot(Player* player, Unit* killer);
        bool RemoveLoot(uint32 playerId, uint32 lootId);
        uint32 GetMaxPlayerLoot() const;
        float GetDropMoneyRate(Player* player) const;
        float GetDropItemsRate(Player* player) const;
        float GetDropGearRate(Player* player) const;
        bool ShouldDropLoot(Player* player, Unit* killer = nullptr);
        bool ShouldDropMoney(Player* player);
        bool ShouldDropItems(Player* player);
        bool ShouldDropGear(Player* player);
        bool IsDropLootEnabled() const;

        // Grave methods
        void RemoveAllGraves();
        void CreateGrave(Player* player, Unit* killer = nullptr);
        bool ShouldSpawnGrave(Player* player, Unit* killer = nullptr);
        bool CanRevive(Player* player);
        bool ShouldReviveOnGraveyard(Player* player);
        void GenerateMissingGraves();
        void PreLoadGraves();
        void LoadGraves();

        // Level methods
        void LevelDown(Player* player, Unit* killer = nullptr);
        bool ShouldLevelDown(Player* player, Unit* killer = nullptr);

    public:
        Unit* GetKiller(Player* player) const;
        void  SetKiller(Player* player, Unit* killer);

    private:
        std::map<uint32, std::map<uint32, HardcorePlayerLoot>> m_playersLoot;
        std::map<uint32, HardcorePlayerGrave> m_playerGraves;
        std::map<uint32, ObjectGuid> m_lastPlayerDeaths;
    };

    static HardcoreModule hardcoreModule;

}
#endif