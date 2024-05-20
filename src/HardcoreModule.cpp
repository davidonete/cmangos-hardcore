#include "HardcoreModule.h"

#include "Entities/Player.h"
#include "Globals/ObjectMgr.h"
#include "Globals/ObjectAccessor.h"
#include "Spells/SpellMgr.h"
#include "SystemConfig.h"
#include "World/World.h"

namespace cmangos_module
{
    bool IsInRaid(Player* player, HardcoreModule* module)
    {
        if (player && player->IsInWorld())
        {
            if (!player->IsBeingTeleported())
            {
                if (const Map* map = player->GetMap())
                {
                    return map->IsRaid();
                }
            }
            else
            {
                // Try to get the map from the killer
                Unit* killer = module->GetKiller(player);
                if (killer)
                {
                    if (const Map* map = killer->GetMap())
                    {
                        return map->IsRaid();
                    }
                }
            }
        }

        return false;
    }

    bool IsInDungeon(Player* player, HardcoreModule* module)
    {
        if (player && player->IsInWorld())
        {
            if (!player->IsBeingTeleported())
            {
                if (const Map* map = player->GetMap())
                {
                    return map->IsDungeon();
                }
            }
            else
            {
                // Try to get the map from the killer
                Unit* killer = module->GetKiller(player);
                if (killer)
                {
                    if (const Map* map = killer->GetMap())
                    {
                        return map->IsDungeon();
                    }
                }
            }
        }

        return false;
    }

    bool IsFairKill(Player* player, Unit* killer)
    {
        if (killer && killer->IsPlayer())
        {
            const uint32 killerLevel = killer->GetLevel();
            const uint32 playerLevel = player->GetLevel();
            return killerLevel <= playerLevel + 3;
        }

        return true;
    }

    HardcoreLootItem::HardcoreLootItem(uint32 id, uint8 amount)
    : m_id(id)
    , m_isGear(false)
    , m_randomPropertyId(0)
    , m_durability(0)
    , m_enchantments(0)
    , m_amount(amount)
    {

    }

    HardcoreLootItem::HardcoreLootItem(uint32 id, uint8 amount, const std::vector<ItemSlot>& slots)
    : m_id(id)
    , m_isGear(false)
    , m_randomPropertyId(0)
    , m_durability(0)
    , m_enchantments(0)
    , m_amount(amount)
    , m_slots(slots)
    {

    }

    HardcoreLootItem::HardcoreLootItem(uint32 id, uint8 amount, uint32 randomPropertyId, uint32 durability, const std::string& enchantments, const std::vector<ItemSlot>& slots)
    : m_id(id)
    , m_isGear(true)
    , m_randomPropertyId(randomPropertyId)
    , m_durability(durability)
    , m_enchantments(enchantments)
    , m_amount(amount)
    , m_slots(slots)
    {

    }

    HardcoreLootItem::HardcoreLootItem(uint32 id, uint8 amount, uint32 randomPropertyId, uint32 durability, const std::string& enchantments)
    : m_id(id)
    , m_isGear(true)
    , m_randomPropertyId(randomPropertyId)
    , m_durability(durability)
    , m_enchantments(enchantments)
    , m_amount(amount)
    {

    }

    HardcoreLootGameObject::HardcoreLootGameObject(uint32 id, uint32 playerId, uint32 lootId, uint32 lootTableId, uint32 money, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const std::vector<HardcoreLootItem>& items, HardcoreModule* module)
    : m_id(id)
    , m_playerId(playerId)
    , m_guid(0)
    , m_lootId(lootId)
    , m_lootTableId(lootTableId)
    , m_money(money)
    , m_positionX(positionX)
    , m_positionY(positionY)
    , m_positionZ(positionZ)
    , m_orientation(orientation)
    , m_mapId(mapId)
    , m_phaseMask(phaseMask)
    , m_items(items)
    , m_module(module)
    {

    }

    HardcoreLootGameObject HardcoreLootGameObject::Load(uint32 id, uint32 playerId, HardcoreModule* module)
    {
        std::vector<HardcoreLootItem> items;
        uint32 lootId, lootTableId, money, mapId, phaseMask;
        float positionX, positionY, positionZ, orientation;

        // Load the gameobject info
        auto result = CharacterDatabase.PQuery("SELECT loot_id, loot_table, money, position_x, position_y, position_z, orientation, map, phase_mask FROM custom_hardcore_loot_gameobjects WHERE id = '%d'", id);
        if (result)
        {
            Field* fields = result->Fetch();
            lootId = fields[0].GetUInt32();
            lootTableId = fields[1].GetUInt32();
            money = fields[2].GetUInt32();
            positionX = fields[3].GetFloat();
            positionY = fields[4].GetFloat();
            positionZ = fields[5].GetFloat();
            orientation = fields[6].GetFloat();
            mapId = fields[7].GetUInt32();
            phaseMask = fields[8].GetUInt32();

            // Load the gameobject items from the custom_hardcore_loot_tables
            auto result2 = CharacterDatabase.PQuery("SELECT item, amount, random_property_id, durability, enchantments FROM custom_hardcore_loot_tables WHERE id = '%d'", lootTableId);
            if (result2)
            {
                do
                {
                    Field* fields = result2->Fetch();
                    const uint32 itemId = fields[0].GetUInt32();
                    const uint8 amount = fields[1].GetUInt8();
                    const uint32 randomPropertyId = fields[2].GetUInt32();
                    const uint32 durability = fields[3].GetUInt32();
                    const std::string enchantments = fields[4].GetString();
                    items.emplace_back(itemId, amount, randomPropertyId, durability, enchantments);
                } 
                while (result2->NextRow());
            }
        }

        return HardcoreLootGameObject(id, playerId, lootId, lootTableId, money, positionX, positionY, positionZ, orientation, mapId, phaseMask, items, module);
    }

    HardcoreLootGameObject HardcoreLootGameObject::Create(uint32 playerId, uint32 lootId, uint32 money, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, const std::vector<HardcoreLootItem>& items, HardcoreModule* module)
    {
        // Generate valid loot table id
        uint32 newLootTableId = 1;
        auto result = CharacterDatabase.PQuery("SELECT id FROM custom_hardcore_loot_tables ORDER BY id DESC LIMIT 1");
        if (result)
        {
            Field* fields = result->Fetch();
            newLootTableId = fields[0].GetUInt32() + 1;
        }

        // Generate valid game object id
        uint32 newGOId = 1;
        auto result3 = CharacterDatabase.PQuery("SELECT id FROM custom_hardcore_loot_gameobjects ORDER BY id DESC LIMIT 1");
        if (result3)
        {
            Field* fields = result3->Fetch();
            newGOId = fields[0].GetUInt32() + 1;
        }

        // Create loot table in custom_hardcore_loot_tables and gameobject_loot_template
        for (const HardcoreLootItem& item : items)
        {
            CharacterDatabase.PExecute("INSERT INTO custom_hardcore_loot_tables (id, item, amount, random_property_id, durability, enchantments) VALUES ('%d', '%d', '%d', '%d', '%d', '%s')",
                newLootTableId,
                item.m_id,
                item.m_amount,
                item.m_randomPropertyId,
                item.m_durability,
                item.m_enchantments.c_str());
        }

        // Create game object in custom_hardcore_loot_gameobjects
        CharacterDatabase.PExecute("INSERT INTO custom_hardcore_loot_gameobjects (id, player, loot_id, loot_table, money, position_x, position_y, position_z, orientation, map, phase_mask) VALUES ('%d', '%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '%d', '%d')",
            newGOId,
            playerId,
            lootId,
            newLootTableId,
            money,
            positionX,
            positionY,
            positionZ,
            orientation,
            mapId,
            phaseMask);

        return HardcoreLootGameObject(newGOId, playerId, lootId, newLootTableId, money, positionX, positionY, positionZ, orientation, mapId, phaseMask, items, module);
    }

    void HardcoreLootGameObject::Spawn()
    {
        if (!IsSpawned())
        {
            const static uint32 lootGOEntry = m_module->GetConfig()->lootGameObjectId;
            const uint32 goLowGUID = sObjectMgr.GenerateStaticGameObjectLowGuid();
            if (goLowGUID)
            {
                Map* map = sMapMgr.FindMap(m_mapId);
                if (!map)
                {
                    const ObjectGuid playerGUID = ObjectGuid(HIGHGUID_PLAYER, m_playerId);
                    if (Player* player = sObjectMgr.GetPlayer(playerGUID))
                    {
                        map = player->GetMap();
                    }
                }

                if (map)
                {
                    GameObject* pGameObject = GameObject::CreateGameObject(lootGOEntry);
    #if EXPANSION == 2
                    if (pGameObject->Create(0, goLowGUID, lootGOEntry, map, m_phaseMask, m_positionX, m_positionY, m_positionZ, m_orientation))
    #else
                    if (pGameObject->Create(0, goLowGUID, lootGOEntry, map, m_positionX, m_positionY, m_positionZ, m_orientation))
    #endif
                    {
                        // Save the chest to the database and load game object data
#if EXPANSION == 0
                    pGameObject->SaveToDB(map->GetId());
#elif EXPANSION == 1
                    pGameObject->SaveToDB(map->GetId(), pGameObject->GetPhaseMask());
#elif EXPANSION == 2
                    GameObjectData const* data = sObjectMgr.GetGOData(pGameObject->GetDbGuid());
                    if (data)
                    {
                        pGameObject->SaveToDB(map->GetId(), data->spawnMask, pGameObject->GetPhaseMask());
                    }
#endif
                        if (pGameObject->LoadFromDB(goLowGUID, map, goLowGUID, 0))
                        {
                            // Set the initial state of the chest to be ready to be looted
                            pGameObject->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                            pGameObject->SetGoState(GO_STATE_READY);
                            pGameObject->SetLootState(GO_READY);
                            pGameObject->SetCooldown(0);

                            // Spawn the loot into the world
                            sObjectMgr.AddGameobjectToGrid(goLowGUID, sObjectMgr.GetGOData(goLowGUID));

                            // Delete the generated object from the database, to prevent duplicate entries
                            WorldDatabase.PExecute("DELETE FROM gameobject WHERE guid = '%d'", goLowGUID);

                            m_guid = goLowGUID;
                        }
                        else
                        {
                            delete pGameObject;
                        }
                    }
                    else
                    {
                        delete pGameObject;
                    }
                }
            }
        }
    }

    void HardcoreLootGameObject::DeSpawn()
    {
        if (IsSpawned())
        {
            if (const GameObjectData* goData = sObjectMgr.GetGOData(m_guid))
            {
                Map* map = sMapMgr.FindMap(m_mapId);
                if (!map)
                {
                    const ObjectGuid playerGUID = ObjectGuid(HIGHGUID_PLAYER, m_playerId);
                    if (Player* player = sObjectMgr.GetPlayer(playerGUID))
                    {
                        map = player->GetMap();
                    }
                }

                if (map)
                {
                    GameObject* obj = map->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, goData->id, m_guid));
                    if (obj)
                    {
                        if (const ObjectGuid& ownerGuid = obj->GetOwnerGuid())
                        {
                            Unit* owner = ownerGuid.IsPlayer() ? ObjectAccessor::FindPlayer(ownerGuid) : nullptr;
                            if (owner)
                            {
                                owner->RemoveGameObject(obj, false);
                            }
                        }

                        obj->SetRespawnTime(0);
                        obj->Delete();
                        obj->DeleteFromDB();

                        m_guid = 0;
                    }
                }
            }
        }
    }

    bool HardcoreLootGameObject::IsSpawned() const
    {
        return m_guid;
    }

    void HardcoreLootGameObject::Destroy()
    {
        DeSpawn();

        // Remove game object from custom_hardcore_loot_gameobjects database
        CharacterDatabase.PExecute("DELETE FROM custom_hardcore_loot_gameobjects WHERE id = '%d'", m_id);

        // Remove loot table from custom_hardcore_loot_tables database
        CharacterDatabase.PExecute("DELETE FROM custom_hardcore_loot_tables WHERE id = '%d'", m_lootTableId);
    }

    void HardcoreLootGameObject::SetMoney(uint32 money)
    {
        m_money = money;
        CharacterDatabase.PExecute("UPDATE custom_hardcore_loot_gameobjects SET money = '%d' WHERE id = '%d'", m_money, m_id);
    }

    const HardcoreLootItem* HardcoreLootGameObject::GetItem(uint32 itemId) const
    {
        for (const HardcoreLootItem& item : m_items)
        {
            if (item.m_id == itemId)
            {
                return &item;
            }
        }

        return nullptr;
    }

    bool HardcoreLootGameObject::RemoveItem(uint32 itemId)
    {
        const HardcoreLootItem* item = GetItem(itemId);
        if (item)
        {
            // Remove the item from the database
            if (CharacterDatabase.DirectPExecute("DELETE FROM custom_hardcore_loot_tables WHERE id = '%d' AND item = '%d'", m_lootTableId, itemId))
            {
                // Remove the item from cache
                m_items.erase(std::remove_if(m_items.begin(), m_items.end(), [&item](const HardcoreLootItem& itemInList)
                {
                    return (item->m_id == itemInList.m_id);
                }), m_items.end());

                return true;
            }
        }

        return false;
    }

    HardcorePlayerLoot::HardcorePlayerLoot(uint32 lootId, uint32 playerId, HardcoreModule* module)
    : m_id(lootId)
    , m_playerId(playerId)
    , m_module(module)
    {

    }

    void HardcorePlayerLoot::LoadGameObject(uint32 gameObjectId)
    {
        m_gameObjects.push_back(std::move(HardcoreLootGameObject::Load(gameObjectId, m_playerId, m_module)));
    }

    HardcoreLootGameObject* HardcorePlayerLoot::FindGameObjectByGUID(const uint32 guid)
    {
        for (HardcoreLootGameObject& gameObject : m_gameObjects)
        {
            if (gameObject.GetGUID() == guid)
            {
                return &gameObject;
            }
        }

        return nullptr;
    }

    bool HardcorePlayerLoot::RemoveGameObject(uint32 gameObjectId)
    {
        for (uint32 i = 0; i < m_gameObjects.size(); i++)
        {
            if (m_gameObjects[i].GetId() == gameObjectId)
            {
                m_gameObjects[i].Destroy();
                m_gameObjects.erase(m_gameObjects.begin() + i);
                return true;
            }
        }

        return false;
    }

    void HardcorePlayerLoot::Spawn()
    {
        // Despawn previously generated gameobjects
        DeSpawn();

        // Spawn the new gameobjects
        for (HardcoreLootGameObject& gameObject : m_gameObjects)
        {
            gameObject.Spawn();
        }
    }

    void HardcorePlayerLoot::DeSpawn()
    {
        for (HardcoreLootGameObject& gameObject : m_gameObjects)
        {
            gameObject.DeSpawn();
        }
    }

    bool HardcorePlayerLoot::Create()
    {
        const ObjectGuid playerGUID = ObjectGuid(HIGHGUID_PLAYER, m_playerId);
        if (Player* player = sObjectMgr.GetPlayer(playerGUID))
        {
            // Generate loot table with the current player's gear
            std::vector<HardcoreLootItem> playerLoot;

            // Hearthstone, Earth Totem, Fire Totem, Water Totem, Air Totem, Ankh
            std::set<uint32> ignoreItems = { 6948, 5175, 5176, 5177, 5178, 17030 };

            auto AddItem = [&player, &ignoreItems](uint8 bag, uint8 slot, std::vector<HardcoreLootItem>& outItems)
            {
                if (Item* pItem = player->GetItemByPos(bag, slot))
                {
                    // Ignore projectiles and quest items 
                    const ItemPrototype* itemData = pItem->GetProto();
                    if ((itemData->Class != ITEM_CLASS_PROJECTILE) && (itemData->Class != ITEM_CLASS_QUEST))
                    {
                        const uint32 itemId = itemData->ItemId;

                        // Ignore items
                        if (ignoreItems.find(itemId) == ignoreItems.end())
                        {
                            // Check if the item exists
                            auto it = std::find_if(outItems.begin(), outItems.end(), [&itemId](const HardcoreLootItem& item)
                            {
                                return item.m_id == itemId;
                            });

                            if (it != outItems.end())
                            {
                                static const uint8 maxAmount = std::numeric_limits<uint8>::max();
                                const uint32 newAmount = (*it).m_amount + pItem->GetCount();
                                (*it).m_amount = (newAmount < maxAmount) ? newAmount : maxAmount;
                                (*it).m_slots.emplace_back(bag, slot);
                            }
                            else
                            {
                                uint32 durability = 0;
                                uint32 randomPropertyId = 0;
                                std::ostringstream enchantments;
                                if (itemData->Class == ITEM_CLASS_WEAPON || itemData->Class == ITEM_CLASS_ARMOR)
                                {
                                    randomPropertyId = pItem->GetItemRandomPropertyId();
                                    durability = pItem->GetUInt32Value(ITEM_FIELD_DURABILITY);

                                    // Get enchantments
                                    for (uint8 i = 0; i < MAX_ENCHANTMENT_SLOT; ++i)
                                    {
                                        enchantments << pItem->GetEnchantmentId(EnchantmentSlot(i)) << ' ';
                                        enchantments << pItem->GetEnchantmentDuration(EnchantmentSlot(i)) << ' ';
                                        enchantments << pItem->GetEnchantmentCharges(EnchantmentSlot(i)) << ' ';
                                    }
                                }

                                std::vector<ItemSlot> slots = { ItemSlot(bag, slot) };
                                outItems.emplace_back(itemId, pItem->GetCount(), randomPropertyId, durability, enchantments.str(), slots);
                            }
                        }
                    }
                }
            };

            auto SelectItemsToDrop = [&player](float dropRate, std::vector<HardcoreLootItem>& items, std::vector<HardcoreLootItem>& outItems)
            {
                if (!items.empty())
                {
                    dropRate = std::min(dropRate, 1.0f);
                    const uint32 dropAmount = items.size() * dropRate;
                    for (uint32 i = 0; i < dropAmount; i++)
                    {
                        // Randomly select an item from the list
                        const uint32 randIdx = urand(0, items.size() - 1);
                        const HardcoreLootItem& item = outItems.emplace_back(items[randIdx]);

                        items.erase(items.begin() + randIdx);

                        // Remove the item from the player (except for bots)
    #ifdef ENABLE_PLAYERBOTS
                        if (player->isRealPlayer())
    #endif
                        {
                            for (const ItemSlot& slot : item.m_slots)
                            {
                                player->DestroyItem(slot.first, slot.second, true);
                            }
                        }
                    }
                }
            };

            // Get player's gear
            if (m_module->ShouldDropGear(player))
            {
                std::vector<HardcoreLootItem> playerGear;

                // Iterate through the player equipment
                for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                {
                    AddItem(INVENTORY_SLOT_BAG_0, slot, playerGear);
                }

                // Generate random list of gear to drop
                SelectItemsToDrop(m_module->GetDropGearRate(player), playerGear, playerLoot);
            }

            // Get player's bag items
            if (m_module->ShouldDropItems(player))
            {
                std::vector<HardcoreLootItem> playerItems;

                // Iterate through the main bag (16 slots)
                for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
                {
                    AddItem(INVENTORY_SLOT_BAG_0, slot, playerItems);
                } 

                // Iterate through the bags (4 bags)
                for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
                {
                    for (uint8 slot = 0; slot < MAX_BAG_SIZE; ++slot)
                    {
                        AddItem(bag, slot, playerItems);
                    }
                }

                // Generate random list of items to drop
                SelectItemsToDrop(m_module->GetDropItemsRate(player), playerItems, playerLoot);
            }

            if (!playerLoot.empty())
            {
                // Split the loot into various game objects (due to loot limitation on client)
                std::vector<std::vector<HardcoreLootItem>> gameObjectsLoot;
                gameObjectsLoot.emplace_back();

                for (const HardcoreLootItem& item : playerLoot)
                {
                    if (gameObjectsLoot.back().size() < MAX_NR_LOOT_ITEMS)
                    {
                        gameObjectsLoot.back().emplace_back(item);
                    }
                    else
                    {
                        gameObjectsLoot.emplace_back();
                        gameObjectsLoot.back().emplace_back(item);
                    }
                }

                // Calculate the amount of money to drop
                uint32 dropMoney = 0;
                if (m_module->ShouldDropMoney(player))
                {
                    const float moneyDropRate = std::min(m_module->GetDropMoneyRate(player), 1.0f);
                    const uint32 playerMoney = player->GetMoney();
                    dropMoney = playerMoney * moneyDropRate;

                    // Remove the money from the player (except for bots)
    #ifdef ENABLE_PLAYERBOTS
                    if (player->isRealPlayer())
    #endif
                    {
                        player->SetMoney(playerMoney - dropMoney);
                    }
                }

                // Create the game objects
                const float playerX = player->GetPositionX();
                const float playerY = player->GetPositionY();
                const float playerZ = player->GetPositionZ();
                const uint32 mapId = player->GetMapId();
    #if EXPANSION == 2
                const uint32 phaseMask = player->GetPhaseMask();
    #else
                const uint32 phaseMask = 0;
    #endif

                const float angleIncrement = (2 * M_PI) / gameObjectsLoot.size();
                static const float radius = 3.0f;
                float angle = 0;

                for (const std::vector<HardcoreLootItem>& items : gameObjectsLoot)
                {
                    // Generate points around the player position
                    float x = playerX + (radius * cos(angle));
                    float y = playerY + (radius * sin(angle));
                    float z = playerZ;
                    float o = atan2(y - playerY, x - playerX);

                    // Check if the height coordinate is valid
                    player->UpdateAllowedPositionZ(x, y, z);

                    // Increment the angle for the next point
                    angle += angleIncrement;

                    HardcoreLootGameObject& gameObject = m_gameObjects.emplace_back(std::move(HardcoreLootGameObject::Create(m_playerId, m_id, dropMoney, x, y, z, o, mapId, phaseMask, items, m_module)));
                
                    // We only want the money to drop once
                    if (dropMoney)
                    {
                        dropMoney = 0;
                    }
                }

                // Spawn generated game objects
                Spawn();

                return true;
            }
        }

        return false;
    }

    void HardcorePlayerLoot::Destroy()
    {
        for (HardcoreLootGameObject& gameObject : m_gameObjects)
        {
            gameObject.Destroy();
        }

        m_gameObjects.clear();
    }

    HardcoreGraveGameObject::HardcoreGraveGameObject(uint32 id, uint32 gameObjectEntry, uint32 playerId, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, HardcoreModule* module)
    : m_id(id)
    , m_gameObjectEntry(gameObjectEntry)
    , m_guid(0)
    , m_playerId(playerId)
    , m_positionX(positionX)
    , m_positionY(positionY)
    , m_positionZ(positionZ)
    , m_orientation(orientation)
    , m_mapId(mapId)
    , m_phaseMask(phaseMask)
    , m_module(module)
    {

    }

    HardcoreGraveGameObject HardcoreGraveGameObject::Load(uint32 id, HardcoreModule* module)
    {
        uint32 gameObjectEntry, playerId, mapId, phaseMask;
        float positionX, positionY, positionZ, orientation;

        auto result = CharacterDatabase.PQuery("SELECT player, gameobject_template, position_x, position_y, position_z, orientation, map, phase_mask FROM custom_hardcore_grave_gameobjects WHERE id = '%d'", id);
        if (result)
        {
            Field* fields = result->Fetch();
            playerId = fields[0].GetUInt32();
            gameObjectEntry = fields[1].GetUInt32();
            positionX = fields[2].GetFloat();
            positionY = fields[3].GetFloat();
            positionZ = fields[4].GetFloat();
            orientation = fields[5].GetFloat();
            mapId = fields[6].GetUInt32();
            phaseMask = fields[7].GetUInt32();
        }

        return HardcoreGraveGameObject(id, gameObjectEntry, playerId, positionX, positionY, positionZ, orientation, mapId, phaseMask, module);
    }

    HardcoreGraveGameObject HardcoreGraveGameObject::Create(uint32 playerId, uint32 gameObjectEntry, float positionX, float positionY, float positionZ, float orientation, uint32 mapId, uint32 phaseMask, HardcoreModule* module)
    {
        // Generate valid game object id
        uint32 newGameObjectId = 1;
        auto result = CharacterDatabase.PQuery("SELECT id FROM custom_hardcore_grave_gameobjects ORDER BY id DESC LIMIT 1");
        if (result)
        {
            Field* fields = result->Fetch();
            newGameObjectId = fields[0].GetUInt32() + 1;
        }

        CharacterDatabase.PExecute("INSERT INTO custom_hardcore_grave_gameobjects (id, player, gameobject_template, position_x, position_y, position_z, orientation, map, phase_mask) VALUES ('%d', '%d', '%d', '%f', '%f', '%f', '%f', '%d', '%d')",
            newGameObjectId,
            playerId,
            gameObjectEntry,
            positionX,
            positionY,
            positionZ,
            orientation,
            mapId,
            phaseMask);

        return HardcoreGraveGameObject(newGameObjectId, gameObjectEntry, playerId, positionX, positionY, positionZ, orientation, mapId, phaseMask, module);
    }

    void HardcoreGraveGameObject::Spawn()
    {
        if (!IsSpawned())
        {
            // Check if the grave gameobject is available
            uint32 gameObjectEntry = m_gameObjectEntry;
            const GameObjectInfo* goInfo = sObjectMgr.GetGameObjectInfo(m_gameObjectEntry);
            if (!goInfo)
            {
                gameObjectEntry = m_module->GetConfig()->graveGameObjectId;
            }

            const uint32 goLowGUID = sObjectMgr.GenerateStaticGameObjectLowGuid();
            if (goLowGUID)
            {
                Map* map = sMapMgr.FindMap(m_mapId);
                if (!map)
                {
                    const ObjectGuid playerGUID = ObjectGuid(HIGHGUID_PLAYER, m_playerId);
                    if (Player* player = sObjectMgr.GetPlayer(playerGUID))
                    {
                        map = player->GetMap();
                    }
                }

                if (map)
                {
                    GameObject* pGameObject = GameObject::CreateGameObject(gameObjectEntry);
    #if EXPANSION == 2
                    if (pGameObject->Create(0, goLowGUID, gameObjectEntry, map, m_phaseMask, m_positionX, m_positionY, m_positionZ, m_orientation))
    #else
                    if (pGameObject->Create(0, goLowGUID, gameObjectEntry, map, m_positionX, m_positionY, m_positionZ, m_orientation))
    #endif
                    {
                        // Save the chest to the database and load game object data
#if EXPANSION == 0
                    pGameObject->SaveToDB(map->GetId());
#elif EXPANSION == 1
                    pGameObject->SaveToDB(map->GetId(), pGameObject->GetPhaseMask());
#elif EXPANSION == 2
                    GameObjectData const* data = sObjectMgr.GetGOData(pGameObject->GetDbGuid());
                    if (data)
                    {
                        pGameObject->SaveToDB(map->GetId(), data->spawnMask, pGameObject->GetPhaseMask());
                    }
#endif
                        if (pGameObject->LoadFromDB(goLowGUID, map, goLowGUID, 0))
                        {
                            // Spawn the loot into the world
                            sObjectMgr.AddGameobjectToGrid(goLowGUID, sObjectMgr.GetGOData(goLowGUID));

                            // Delete the generated object from the database, to prevent duplicate entries
                            WorldDatabase.PExecute("DELETE FROM gameobject WHERE guid = '%d'", goLowGUID);

                            m_guid = goLowGUID;
                        }
                        else
                        {
                            delete pGameObject;
                        }
                    }
                    else
                    {
                        delete pGameObject;
                    }
                }
            }
        }
    }

    void HardcoreGraveGameObject::DeSpawn()
    {
        if (IsSpawned())
        {
            if (const GameObjectData* goData = sObjectMgr.GetGOData(m_guid))
            {
                Map* map = sMapMgr.FindMap(m_mapId);
                if (!map)
                {
                    const ObjectGuid playerGUID = ObjectGuid(HIGHGUID_PLAYER, m_playerId);
                    if (Player* player = sObjectMgr.GetPlayer(playerGUID))
                    {
                        map = player->GetMap();
                    }
                }

                if (map)
                {
                    GameObject* obj = map->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, goData->id, m_guid));
                    if (obj)
                    {
                        if (const ObjectGuid& ownerGuid = obj->GetOwnerGuid())
                        {
                            Unit* owner = ownerGuid.IsPlayer() ? ObjectAccessor::FindPlayer(ownerGuid) : nullptr;
                            if (owner)
                            {
                                owner->RemoveGameObject(obj, false);
                            }
                        }

                        obj->SetRespawnTime(0);
                        obj->Delete();
                        obj->DeleteFromDB();

                        m_guid = 0;
                    }
                }
            }
        }
    }

    bool HardcoreGraveGameObject::IsSpawned() const
    {
        return m_guid;
    }

    void HardcoreGraveGameObject::Destroy()
    {
        DeSpawn();

        // Remove game object from custom_hardcore_grave_gameobjects database
        CharacterDatabase.PExecute("DELETE FROM custom_hardcore_grave_gameobjects WHERE id = '%d'", m_id);
    }

    HardcorePlayerGrave::HardcorePlayerGrave(uint32 playerId, uint32 gameObjectEntry, const std::vector<HardcoreGraveGameObject>& gameObjects, HardcoreModule* module)
    : m_playerId(playerId)
    , m_gameObjectEntry(gameObjectEntry)
    , m_gameObjects(gameObjects)
    , m_module(module)
    {
    
    }

    HardcorePlayerGrave::HardcorePlayerGrave(uint32 playerId, uint32 gameObjectEntry, HardcoreModule* module)
    : m_playerId(playerId)
    , m_gameObjectEntry(gameObjectEntry)
    , m_module(module)
    {
    
    }

    HardcorePlayerGrave HardcorePlayerGrave::Load(uint32 playerId, uint32 gameObjectEntry, HardcoreModule* module)
    {
        std::vector<HardcoreGraveGameObject> gameObjects;
        auto result = CharacterDatabase.PQuery("SELECT id FROM custom_hardcore_grave_gameobjects WHERE player = '%d'", playerId);
        if (result)
        {
            do 
            {
                Field* fields = result->Fetch();
                const uint32 gameObjectId = fields[0].GetUInt32();
                gameObjects.push_back(std::move(HardcoreGraveGameObject::Load(gameObjectId, module)));
            } 
            while (result->NextRow());
        }

        return HardcorePlayerGrave(playerId, gameObjectEntry, gameObjects, module);
    }

    HardcorePlayerGrave HardcorePlayerGrave::Generate(uint32 playerId, const std::string& playerName, HardcoreModule* module)
    {
        // Generate valid game object entry
        uint32 newGameObjectEntry = 0;
        auto result = WorldDatabase.PQuery("SELECT entry FROM gameobject_template ORDER BY entry DESC LIMIT 1");
        if (result)
        {
            Field* fields = result->Fetch();
            newGameObjectEntry = fields[0].GetUInt32() + 1;
        }

        if (newGameObjectEntry)
        {
            // Get gameobject info from existing gameobject from config
            float size = 1.29f;
            uint32 displayId = 12;
            const GameObjectInfo* goInfo = ObjectMgr::GetGameObjectInfo(module->GetConfig()->graveGameObjectId);
            if (goInfo)
            {
                displayId = goInfo->displayId;
                size = goInfo->size;
            }

            std::string graveMessage = GenerateGraveMessage(playerName, module);
            WorldDatabase.PExecute("INSERT INTO gameobject_template (entry, type, displayId, name, size, data10, CustomData1) VALUES ('%d', '%d', '%d', '%s', '%f', '%d', '%d')",
                newGameObjectEntry, 
                2, 
                displayId, 
                graveMessage.c_str(), 
                size,
                playerId,
                3643);
        }

        return HardcorePlayerGrave(playerId, newGameObjectEntry, module);
    }

    void HardcorePlayerGrave::Spawn()
    {
        // Despawn previously generated gameobjects
        DeSpawn();

        // Spawn the new gameobjects
        for (HardcoreGraveGameObject& gameObject : m_gameObjects)
        {
            gameObject.Spawn();
        }
    }

    void HardcorePlayerGrave::DeSpawn()
    {
        for (HardcoreGraveGameObject& gameObject : m_gameObjects)
        {
            gameObject.DeSpawn();
        }
    }

    void HardcorePlayerGrave::Create()
    {
        const ObjectGuid playerGUID = ObjectGuid(HIGHGUID_PLAYER, m_playerId);
        if (Player* player = sObjectMgr.GetPlayer(playerGUID))
        {
            // Create the game objects
            const float x = player->GetPositionX();
            const float y = player->GetPositionY();
            float z = player->GetPositionZ();
            const float o = player->GetOrientation();
            const uint32 mapId = player->GetMapId();
    #if EXPANSION == 2
            const uint32 phaseMask = player->GetPhaseMask();
    #else
            const uint32 phaseMask = 0;
    #endif

            // Check if the height coordinate is valid
            player->UpdateAllowedPositionZ(x, y, z);

            HardcoreGraveGameObject& gameObject = m_gameObjects.emplace_back(std::move(HardcoreGraveGameObject::Create(m_playerId, m_gameObjectEntry, x, y, z, o, mapId, phaseMask, m_module)));
            gameObject.Spawn();
        }
    }

    void HardcorePlayerGrave::Destroy()
    {
        for (HardcoreGraveGameObject& gameObject : m_gameObjects)
        {
            gameObject.Destroy();
        }

        m_gameObjects.clear();

        // Remove game object from gameobject_template database
        WorldDatabase.PExecute("DELETE FROM gameobject_template WHERE entry = '%d'", m_gameObjectEntry);
    }

    std::string HardcorePlayerGrave::GenerateGraveMessage(const std::string& playerName, HardcoreModule* module)
    {
        std::string gravestoneMessage;
        std::string gravestoneMessages = module->GetConfig()->graveMessage;

        // Check if we have multiple messages to select from
        char separator = '|';
        if (gravestoneMessages.find(separator) != std::string::npos)
        {
            std::string segment;
            std::stringstream messages(gravestoneMessages);
            std::vector<std::string> gravestoneMessageList;
            while (std::getline(messages, segment, separator))
            {
                gravestoneMessageList.push_back(segment);
            }

            if(!gravestoneMessageList.empty())
            {
                const size_t messageIndex = urand(0, gravestoneMessageList.size() - 1);
                gravestoneMessage = gravestoneMessageList[messageIndex];
            }
            else
            {
                gravestoneMessage = "Here lies <PlayerName>";
            }
        }
        else
        {
            gravestoneMessage = gravestoneMessages;
        }

        // Replace values on the message
        static const std::string playerNameVar = "<PlayerName>";
        const size_t startPos = gravestoneMessage.find(playerNameVar);
        if (startPos != std::string::npos)
        {
            gravestoneMessage.replace(startPos, playerNameVar.length(), playerName);
        }

        return gravestoneMessage;
    }

    void HardcoreModule::OnCharacterCreated(Player* player)
    {
        // Generate player grave gameobject
        if (GetConfig()->enabled && GetConfig()->spawnGrave)
        {
            const uint32 playerId = player->GetObjectGuid().GetCounter();

    #ifdef ENABLE_PLAYERBOTS
            // Check if the player is not a bot
            Config config;
            if (config.SetSource(SYSCONFDIR"aiplayerbot.conf", ""))
            {
                std::string botPrefix = config.GetStringDefault("AiPlayerbot.RandomBotAccountPrefix", "rndbot");
                std::transform(botPrefix.begin(), botPrefix.end(), botPrefix.begin(), ::toupper);

                uint32 playerAccountId = player->GetSession()->GetAccountId();
                auto result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = '%d'", playerAccountId);
                if (result)
                {
                    Field* fields = result->Fetch();
                    const std::string accountName = fields[0].GetCppString();
                    if (accountName.find(botPrefix) != std::string::npos)
                        return;
                }
            }
    #endif

            // Check if the player grave exists
            if (m_playerGraves.find(playerId) == m_playerGraves.end())
            {
                m_playerGraves.insert(std::make_pair(playerId, HardcorePlayerGrave::Generate(playerId, player->GetName(), this)));
            }
        }
    }

    void HardcoreModule::OnDeleteFromDB(uint32 playerId)
    {
        // Delete player grave
        auto graveIt = m_playerGraves.find(playerId);
        if (graveIt != m_playerGraves.end())
        {
            graveIt->second.Destroy();
            m_playerGraves.erase(graveIt);
        }

        // Delete player loot
        auto playerLootIt = m_playersLoot.find(playerId);
        if (playerLootIt != m_playersLoot.end())
        {
            for (auto lootIt = playerLootIt->second.begin(); lootIt != playerLootIt->second.end(); ++lootIt)
            {
                lootIt->second.Destroy();
            }

            m_playersLoot.erase(playerLootIt);
        }
    }

    bool HardcoreModule::OnPreResurrect(Player* player)
    {
        // Prevent resurrecting
        return !CanRevive(player);
    }

    HardcoreModule::HardcoreModule()
    : Module("Hardcore", new HardcoreModuleConfig())
    {
        
    }

    const HardcoreModuleConfig* HardcoreModule::GetConfig() const
    {
        return (HardcoreModuleConfig*)Module::GetConfig();
    }

    void HardcoreModule::OnWorldPreInitialized()
    {
        if (GetConfig()->enabled)
        {
            PreLoadLoot();
            PreLoadGraves();
        }
    }

    void HardcoreModule::OnInitialize()
    {
        if (GetConfig()->enabled)
        {
            LoadLoot();
            LoadGraves();
        }
    }

    void HardcoreModule::OnResurrect(Player* player)
    {
        // See if we can retrieve the killer
        Unit* killer = GetKiller(player);

        // Process level down
        LevelDown(player, killer);

        // Clear the player killer
        SetKiller(player, nullptr);
    }

    void HardcoreModule::OnDeath(Player* player, Unit* killer)
    {
        if (GetConfig()->enabled)
        {
            // Check if the killer is a pet and if so get the owner
            if (killer && killer->IsCreature() && killer->GetOwner())
            {
                killer = killer->GetOwner();
            }

            // Save the player killer for later use
            SetKiller(player, killer);

            // Process loot and grave spawning
            CreateLoot(player, killer);
            CreateGrave(player, killer);
        }
    }

    void HardcoreModule::OnReleaseSpirit(Player* player, const WorldSafeLocsEntry* closestGrave)
    {
        const bool teleportedToGraveyard = closestGrave != nullptr;
        if (player && teleportedToGraveyard && ShouldReviveOnGraveyard(player))
        {
            player->ResurrectPlayer(1.0f);
            player->SpawnCorpseBones();

            // Apply temporary immune aura
            const SpellEntry* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(1020);
            if (spellInfo)
            {
                SpellAuraHolder* holder = CreateSpellAuraHolder(spellInfo, player, player);
                for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    uint8 eff = spellInfo->Effect[i];
                    if (eff >= MAX_SPELL_EFFECTS)
                    {
                        continue;
                    }

                    if (IsAreaAuraEffect(eff) ||
                        eff == SPELL_EFFECT_APPLY_AURA ||
                        eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                    {
                        int32 basePoints = spellInfo->CalculateSimpleValue(SpellEffectIndex(i));
                        int32 damage = basePoints;
                        Aura* aur = CreateAura(spellInfo, SpellEffectIndex(i), &damage, &basePoints, holder, player);
                        holder->AddAura(aur, SpellEffectIndex(i));
                    }
                }

                if (!player->AddSpellAuraHolder(holder))
                {
                    delete holder;
                }
            }
        }
    }

    void HardcoreModule::PreLoadLoot()
    {
        if (IsDropLootEnabled())
        {
            // Load the loot game objects and loot tables
            auto result = CharacterDatabase.Query("SELECT id, player, loot_id FROM custom_hardcore_loot_gameobjects");
            if (result)
            {
                do
                {
                    Field* fields = result->Fetch();
                    const uint32 gameObjectId = fields[0].GetUInt32();
                    const uint32 playerId = fields[1].GetUInt32();
                    const uint32 lootId = fields[2].GetUInt32();

                    // Check if the players loot exists
                    if (m_playersLoot.find(playerId) == m_playersLoot.end())
                    {
                        // If not add it to the map
                        m_playersLoot.insert(std::make_pair(playerId, std::map<uint32, HardcorePlayerLoot>()));
                    }

                    // Check if the loot exists
                    auto& playerLoots = m_playersLoot.at(playerId);
                    if (playerLoots.find(lootId) == playerLoots.end())
                    {
                        // If not add it to the player loot
                        playerLoots.insert(std::make_pair(lootId, HardcorePlayerLoot(lootId, playerId, this)));
                    }

                    // Load the game object
                    HardcorePlayerLoot& playerLoot = playerLoots.at(lootId);
                    playerLoot.LoadGameObject(gameObjectId);
                }
                while (result->NextRow());
            }
        }
    }

    void HardcoreModule::LoadLoot()
    {
        if (IsDropLootEnabled())
        {
            // Add the preloaded loot gameobjects into the world
            for (auto& pair : m_playersLoot)
            {
                for (auto& pair2 : pair.second)
                {
                    pair2.second.Spawn();
                }
            }
        }
    }

    void HardcoreModule::GenerateMissingGraves()
    {
        if (GetConfig()->enabled && GetConfig()->spawnGrave)
        {
            auto result = CharacterDatabase.Query("SELECT guid, account, name FROM characters");
            if (result)
            {
                do
                {
                    Field* fields = result->Fetch();
                    const uint32 playerId = fields[0].GetUInt32();
                    const uint32 playerAccountId = fields[1].GetUInt32();
                    const std::string playerName = fields[2].GetCppString();
                
    #ifdef ENABLE_PLAYERBOTS
                    // Check if the player is not a bot
                    Config config;
                    if (config.SetSource(SYSCONFDIR"aiplayerbot.conf", ""))
                    {
                        std::string botPrefix = config.GetStringDefault("AiPlayerbot.RandomBotAccountPrefix", "rndbot");
                        std::transform(botPrefix.begin(), botPrefix.end(), botPrefix.begin(), ::toupper);

                        auto result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = '%d'", playerAccountId);
                        if (result)
                        {
                            Field* fields = result->Fetch();
                            const std::string accountName = fields[0].GetCppString();
                            if (accountName.find(botPrefix) != std::string::npos)
                                return;
                        }
                    }
    #endif

                    // Check if the player grave exists
                    if (m_playerGraves.find(playerId) == m_playerGraves.end())
                    {
                        m_playerGraves.insert(std::make_pair(playerId, HardcorePlayerGrave::Generate(playerId, playerName, this)));
                    }
                }
                while (result->NextRow());
            }
        }
    }

    HardcoreLootGameObject* HardcoreModule::FindLootGOByGUID(const uint32 guid)
    {
        for (auto it = m_playersLoot.begin(); it != m_playersLoot.end(); ++it)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                HardcoreLootGameObject* lootGameObject = it2->second.FindGameObjectByGUID(guid);
                if (lootGameObject)
                {
                    return lootGameObject;
                }
            }
        }

        return nullptr;
    }

    HardcorePlayerLoot* HardcoreModule::FindLootByID(const uint32 playerId, const uint32 lootId)
    {
        auto playerLootsIt = m_playersLoot.find(playerId);
        if (playerLootsIt != m_playersLoot.end())
        {
            std::map<uint32, HardcorePlayerLoot>& playerLoots = playerLootsIt->second;
            auto playerLootIt = playerLoots.find(lootId);
            if (playerLootIt != playerLoots.end())
            {
                return &playerLootIt->second;
            }
        }

        return nullptr;
    }

    void HardcoreModule::CreateLoot(Player* player, Unit* killer)
    {
        if (player && ShouldDropLoot(player, killer))
        {
            const uint32 playerId = player->GetObjectGuid().GetCounter();

            // Check if we need to remove a previous loot before creating a new one
            auto playerLootsIt = m_playersLoot.find(playerId);
            if (playerLootsIt != m_playersLoot.end())
            {
                std::map<uint32, HardcorePlayerLoot>& playerLoots = playerLootsIt->second;
                if (playerLoots.size() >= GetMaxPlayerLoot())
                {
                    // Get the oldest loot to remove
                    HardcorePlayerLoot& playerLoot = playerLoots.begin()->second;
                    RemoveLoot(playerLoot.GetPlayerId(), playerLoot.GetId());
                }
            }

            // Check if the players loot exists
            if (m_playersLoot.find(playerId) == m_playersLoot.end())
            {
                // If not add it to the map
                m_playersLoot.insert(std::make_pair(playerId, std::map<uint32, HardcorePlayerLoot>()));
            }

            std::map<uint32, HardcorePlayerLoot>& playerLoots = m_playersLoot.at(playerId);

            // Generate valid loot id
            uint32 newLootId = 1;
            auto result = CharacterDatabase.PQuery("SELECT loot_id FROM custom_hardcore_loot_gameobjects WHERE player = '%d' ORDER BY loot_id DESC LIMIT 1", playerId);
            if (result)
            {
                Field* fields = result->Fetch();
                newLootId = fields[0].GetUInt32() + 1;
            }

            // Generate new loot
            playerLoots.insert(std::make_pair(newLootId, HardcorePlayerLoot(newLootId, playerId, this)));
            HardcorePlayerLoot& playerLoot = playerLoots.at(newLootId);
            if (!playerLoot.Create())
            {
                // Remove the loot if failed to create
                playerLoots.erase(newLootId);
            }
        }
    }

    bool HardcoreModule::RemoveLoot(uint32 playerId, uint32 lootId)
    {
        HardcorePlayerLoot* playerLoot = FindLootByID(playerId, lootId);
        if (playerLoot)
        {
            std::map<uint32, HardcorePlayerLoot>& playerLoots = m_playersLoot.at(playerId);
            playerLoot->Destroy();
            playerLoots.erase(lootId);

            if (playerLoots.empty())
            {
                m_playersLoot.erase(playerId);
            }

            return true;
        }

        return false;
    }

    void HardcoreModule::RemoveAllLoot()
    {
        for (auto& pair : m_playersLoot)
        {
            for (auto& pair2 : pair.second)
            {
                pair2.second.Destroy();
            }
        }

        m_playersLoot.clear();
    }

    bool HardcoreModule::OnFillLoot(Loot* loot, Player* owner)
    {
        if (GetConfig()->enabled && IsDropLootEnabled())
        {
            if (loot && loot->GetLootTarget() && loot->GetLootTarget()->IsGameObject())
            {
                // Look for the items in the loot cache
                const HardcoreLootGameObject* lootGameObject = FindLootGOByGUID(loot->GetLootTarget()->GetGUIDLow());
                if (lootGameObject)
                {
                    for (const HardcoreLootItem& item : lootGameObject->GetItems())
                    {
                        loot->AddItem(item.m_id, item.m_amount, 0, item.m_randomPropertyId);
                    }

                    return true;
                }
            }
        }

        return false;
    }

    bool HardcoreModule::OnGenerateMoneyLoot(Loot* loot, uint32& outMoney)
    {
        if (GetConfig()->enabled && IsDropLootEnabled())
        {
            if (loot && loot->GetLootTarget() && loot->GetLootTarget()->IsGameObject())
            {
                // Look for the items in the loot cache
                HardcoreLootGameObject* lootGameObject = FindLootGOByGUID(loot->GetLootTarget()->GetGUIDLow());
                if (lootGameObject)
                {
                    outMoney = lootGameObject->GetMoney();
                    return true;
                }
            }
        }

        return false;
    }

    void HardcoreModule::OnAddItem(Loot* loot, LootItem* lootItem)
    {
        if (GetConfig()->enabled && IsDropLootEnabled())
        {
            if (loot && lootItem && loot->GetLootTarget() && loot->GetLootTarget()->IsGameObject())
            {
                // Look for the items in the loot cache
                HardcoreLootGameObject* lootGameObject = FindLootGOByGUID(loot->GetLootTarget()->GetGUIDLow());
                if (lootGameObject)
                {
                    // Remove the allowed guids to allow anybody to loot this
                    lootItem->allowedGuid.clear();
                }
            }
        }
    }

    void HardcoreModule::OnSendGold(Loot* loot, Player* player, uint32 gold, uint8 lootMethod)
    {
        if (GetConfig()->enabled && IsDropLootEnabled())
        {
            if (loot && loot->GetLootTarget() && loot->GetLootTarget()->IsGameObject())
            {
                // Look for the items in the loot cache
                HardcoreLootGameObject* lootGameObject = FindLootGOByGUID(loot->GetLootTarget()->GetGUIDLow());
                if (lootGameObject)
                {
                    lootGameObject->SetMoney(0);
                }
            }
        }
    }

    std::vector<ModuleChatCommand>* HardcoreModule::GetCommandTable()
    {
        static std::vector<ModuleChatCommand> commandTable =
        {
            { "reset", std::bind(&HardcoreModule::HandleResetCommand, this, std::placeholders::_1, std::placeholders::_2), SEC_GAMEMASTER },
            { "resetgraves", std::bind(&HardcoreModule::HandleResetGravesCommand, this, std::placeholders::_1, std::placeholders::_2), SEC_GAMEMASTER },
            { "resetloot", std::bind(&HardcoreModule::HandleResetLootCommand, this, std::placeholders::_1, std::placeholders::_2), SEC_GAMEMASTER },
            { "spawnloot", std::bind(&HardcoreModule::HandleSpawnLootCommand, this, std::placeholders::_1, std::placeholders::_2), SEC_GAMEMASTER },
            { "spawngrave", std::bind(&HardcoreModule::HandleSpawnGraveCommand, this, std::placeholders::_1, std::placeholders::_2), SEC_GAMEMASTER },
            { "leveldown", std::bind(&HardcoreModule::HandleLevelDownCommand, this, std::placeholders::_1, std::placeholders::_2), SEC_GAMEMASTER },
        };

        return &commandTable;
    }

    bool HardcoreModule::HandleResetCommand(WorldSession* session, const std::string& args)
    {
        if (GetConfig()->enabled)
        {
            RemoveAllLoot();
            RemoveAllGraves();
            return true;
        }

        return false;
    }

    bool HardcoreModule::HandleResetGravesCommand(WorldSession* session, const std::string& args)
    {
        if (GetConfig()->enabled)
        {
            RemoveAllGraves();
            return true;
        }

        return false;
    }

    bool HardcoreModule::HandleResetLootCommand(WorldSession* session, const std::string& args)
    {
        if (GetConfig()->enabled)
        {
            RemoveAllLoot();
            return true;
        }

        return false;
    }

    bool HardcoreModule::HandleSpawnLootCommand(WorldSession* session, const std::string& args)
    {
        if (GetConfig()->enabled)
        {
            if (session && session->GetPlayer())
            {
                CreateLoot(session->GetPlayer(), nullptr);
                return true;
            }
        }

        return false;
    }

    bool HardcoreModule::HandleSpawnGraveCommand(WorldSession* session, const std::string& args)
    {
        if (GetConfig()->enabled)
        {
            if (session && session->GetPlayer())
            {
                CreateGrave(session->GetPlayer());
                return true;
            }
        }

        return false;
    }

    bool HardcoreModule::HandleLevelDownCommand(WorldSession* session, const std::string& args)
    {
        if (GetConfig()->enabled)
        {
            if (session && session->GetPlayer())
            {
                LevelDown(session->GetPlayer());
                return true;
            }
        }
        
        return false;
    }

    void HardcoreModule::OnStoreItem(Player* player, Loot* loot, Item* item)
    {
        if (GetConfig()->enabled && IsDropLootEnabled())
        {
            if (loot && item && loot->GetLootTarget() && loot->GetLootTarget()->IsGameObject())
            {
                // Look for the items in the loot cache
                HardcoreLootGameObject* lootGameObject = FindLootGOByGUID(loot->GetLootTarget()->GetGUIDLow());
                if (lootGameObject)
                {
                    const HardcoreLootItem* hardcoreItem = lootGameObject->GetItem(item->GetProto()->ItemId);
                    if (hardcoreItem)
                    {
                        if (!hardcoreItem->m_enchantments.empty() || (hardcoreItem->m_durability > 0))
                        {
                            // Set the enchantments
                            if (!hardcoreItem->m_enchantments.empty())
                            {
    #if EXPANSION == 0
                                item->_LoadIntoDataField(hardcoreItem->m_enchantments.c_str(), ITEM_FIELD_ENCHANTMENT, MAX_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET);
    #else
                                item->_LoadIntoDataField(hardcoreItem->m_enchantments.c_str(), ITEM_FIELD_ENCHANTMENT_1_1, MAX_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET);
    #endif
                            }

                            // Set the durability
                            if (hardcoreItem->m_durability > 0)
                            {
                                item->SetUInt32Value(ITEM_FIELD_DURABILITY, hardcoreItem->m_durability);
                            }
                        }

                        // Remove the item from the loot table
                        if (lootGameObject->RemoveItem(hardcoreItem->m_id))
                        {
                            // If we don't have items left in the loot gameobject remove it
                            if (!lootGameObject->HasItems())
                            {
                                const uint32 lootId = lootGameObject->GetLootId();
                                const uint32 playerId = lootGameObject->GetPlayerId();

                                HardcorePlayerLoot* playerLoot = FindLootByID(playerId, lootId);
                                if (playerLoot)
                                {
                                    if (playerLoot->RemoveGameObject(lootGameObject->GetId()))
                                    {
                                        // Check if all the gameobjects have been looted
                                        if (!playerLoot->HasGameObjects())
                                        {
                                            RemoveLoot(playerId, lootId);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    bool HardcoreModule::ShouldDropLoot(Player* player, Unit* killer /*= nullptr*/)
    {
        if (GetConfig()->enabled)
        {
            if (player)
            {
#ifdef ENABLE_PLAYERBOTS
                // Only drop loot if a bot gets killed by a real player
                if (!player->isRealPlayer())
                {
                    if (!killer || (killer->IsCreature() || (killer->IsPlayer() && !((Player*)killer)->isRealPlayer())))
                    {
                        return false;
                    }
                }
#endif

                const uint32 playerLevel = player->GetLevel();
                if (playerLevel < GetConfig()->dropMinLevel || playerLevel >= GetConfig()->dropMaxLevel)
                {
                    return false;
                }

                if (!GetConfig()->dropOnDungeons && IsInDungeon(player, this))
                {
                    return false;
                }
                
                if (!GetConfig()->dropOnRaids && IsInRaid(player, this))
                {
                    return false;
                }

                return IsFairKill(player, killer) && !helper::InPvpMap(player) && (ShouldDropGear(player) || ShouldDropItems(player) || ShouldDropMoney(player));
            }
        }

        return false;
    }

    bool HardcoreModule::ShouldDropMoney(Player* player)
    {
        if (GetConfig()->enabled)
        {
            if (!helper::InPvpMap(player))
            {
                return GetDropMoneyRate(player);
            }
        }

        return false;
    }

    bool HardcoreModule::ShouldDropItems(Player* player)
    {
        if (GetConfig()->enabled)
        {
            if (!helper::InPvpMap(player))
            {
                return GetDropItemsRate(player);
            }
        }

        return false;
    }

    bool HardcoreModule::ShouldDropGear(Player* player)
    {
        if (GetConfig()->enabled)
        {
            if (player)
            {
                if (!helper::InPvpMap(player))
                {
                    return GetDropGearRate(player);
                }
            }
        }

        return false;
    }

    bool HardcoreModule::CanRevive(Player* player)
    {
        if (GetConfig()->enabled)
        {
            if (player && GetConfig()->reviveDisabled)
            {
    #ifdef ENABLE_PLAYERBOTS
                // Bots always should revive
                if (!player->isRealPlayer())
                    return true;
    #endif

                return helper::InPvpMap(player);
            }
        }

        return true;
    }

    bool HardcoreModule::ShouldReviveOnGraveyard(Player* player)
    {
        if (GetConfig()->enabled)
        {
            if (player && CanRevive(player) && GetConfig()->reviveOnGraveyard)
            {
    #ifdef ENABLE_PLAYERBOTS
                // Bots always should revive using ghosts
                if (!player->isRealPlayer())
                    return false;
    #endif

                return !helper::InPvpMap(player) && !IsInDungeon(player, this) && !IsInRaid(player, this);
            }
        }

        return false;
    }

    bool HardcoreModule::ShouldLevelDown(Player* player, Unit* killer /*= nullptr*/)
    {
        if (GetConfig()->enabled)
        {
            if (player && GetConfig()->levelDownPct > 0.0f)
            {
#ifdef ENABLE_PLAYERBOTS
                // Bots should never level down
                if (!player->isRealPlayer())
                    return false;
#endif

                const uint32 playerLevel = player->GetLevel();
                if (playerLevel < GetConfig()->levelDownMinLevel || playerLevel >= GetConfig()->levelDownMaxLevel)
                {
                    return false;
                }

                if (!GetConfig()->levelDownOnDungeons && IsInDungeon(player, this))
                {
                    return false;
                }

                if (!GetConfig()->levelDownOnRaids && IsInRaid(player, this))
                {
                    return false;
                }

                return !helper::InPvpMap(player) && IsFairKill(player, killer);
            }
        }

        return false;
    }

    uint32 HardcoreModule::GetMaxPlayerLoot() const
    {
        const uint32 maxPlayerLoot = GetConfig()->maxDroppedLoot;
        return maxPlayerLoot > 0 ? maxPlayerLoot : 1;
    }

    float HardcoreModule::GetDropMoneyRate(Player* player) const
    {
    #ifdef ENABLE_PLAYERBOTS
        const bool isBot = player ? !player->isRealPlayer() : false;
        return isBot ? GetConfig()->botDropMoneyPct : GetConfig()->dropMoneyPct;
    #else
        return GetConfig()->dropMoneyPct;
    #endif
    }

    float HardcoreModule::GetDropItemsRate(Player* player) const
    {
    #ifdef ENABLE_PLAYERBOTS
        const bool isBot = player ? !player->isRealPlayer() : false;
        return isBot ? GetConfig()->botDropItemsPct : GetConfig()->dropItemsPct;
    #else
        return GetConfig()->dropItemsPct;
    #endif
    }

    float HardcoreModule::GetDropGearRate(Player* player) const
    {
    #ifdef ENABLE_PLAYERBOTS
        const bool isBot = player ? !player->isRealPlayer() : false;
        return isBot ? GetConfig()->botDropGearPct : GetConfig()->dropGearPct;
    #else
        return GetConfig()->dropGearPct;
    #endif
    }

    bool HardcoreModule::ShouldSpawnGrave(Player* player, Unit* killer /*= nullptr*/)
    {
        if (GetConfig()->enabled && GetConfig()->spawnGrave)
        {
            if (player)
            {
#ifdef ENABLE_PLAYERBOTS
                // Bots should never spawn a grave
                if (!player->isRealPlayer())
                    return false;
#endif

                const uint32 playerLevel = player->GetLevel();
                if (playerLevel < GetConfig()->dropMinLevel || playerLevel >= GetConfig()->dropMaxLevel)
                {
                    return false;
                }

                if (!GetConfig()->dropOnDungeons && IsInDungeon(player, this))
                {
                    return false;
                }

                if (!GetConfig()->dropOnRaids && IsInRaid(player, this))
                {
                    return false;
                }

                return !helper::InPvpMap(player) && IsFairKill(player, killer);
            }
        }

        return false;
    }

    void HardcoreModule::PreLoadGraves()
    {
        if (GetConfig()->enabled && GetConfig()->spawnGrave)
        {
            // Load player graves from db
            auto result = WorldDatabase.PQuery("SELECT entry, data10 FROM gameobject_template WHERE type = '%d' AND CustomData1 = '%d'", 2, 3643);
            if (result)
            {
                do
                {
                    Field* fields = result->Fetch();
                    const uint32 gameObjectEntry = fields[0].GetUInt32();
                    const uint32 playerId = fields[1].GetUInt32();

                    if (m_playerGraves.find(playerId) == m_playerGraves.end())
                    {
                        // If not load player grave
                        m_playerGraves.insert(std::make_pair(playerId, HardcorePlayerGrave::Load(playerId, gameObjectEntry, this)));
                    }
                }
                while (result->NextRow());
            }

            // Generate missing graves
            GenerateMissingGraves();
        }
    }

    void HardcoreModule::LoadGraves()
    {
        if (GetConfig()->enabled && GetConfig()->spawnGrave)
        {
            // Add the preloaded graves gameobjects into the world
            for (auto& pair : m_playerGraves)
            {
                pair.second.Spawn();
            }
        }
    }

    void HardcoreModule::CreateGrave(Player* player, Unit* killer)
    {
        if (player && ShouldSpawnGrave(player, killer))
        {
            // Check if the player grave exists
            const uint32 playerId = player->GetObjectGuid().GetCounter();
            auto it = m_playerGraves.find(playerId);
            if (it != m_playerGraves.end())
            {
                it->second.Create();
            }
        }
    }

    void HardcoreModule::RemoveAllGraves()
    {
        for (auto& pair : m_playerGraves)
        {
            pair.second.Destroy();
        }

        m_playerGraves.clear();
    }

    void HardcoreModule::LevelDown(Player* player, Unit* killer)
    {
        if (player && ShouldLevelDown(player, killer))
        {
            // Calculate how many levels and XP (%) we have to remove
            const float levelDownRate = GetConfig()->levelDownPct;
            uint32 totalLevelXP = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
            uint32 curXP = player->GetUInt32Value(PLAYER_XP);
            totalLevelXP = totalLevelXP ? totalLevelXP : 1;
            const float levelPct = (float)(curXP) / totalLevelXP;
            const float level = player->GetLevel() + levelPct;
            const float levelDown = level - levelDownRate;

            // Separate the amount of levels and the XP (%)
            double newLevel, newXPpct;
            newXPpct = modf(levelDown, &newLevel);
            newLevel = newLevel < 0.0 ? 1.0 : newLevel;

            // Process the level down
            if (newLevel > 0.0)
            {
                player->GiveLevel((uint32)newLevel);
                player->InitTalentForLevel();
                player->SetUInt32Value(PLAYER_XP, 0);
            }

            // Process the XP down
            if (newXPpct > 0.0)
            {
                curXP = player->GetUInt32Value(PLAYER_XP);
                totalLevelXP = sObjectMgr.GetXPForLevel(newLevel);

                const uint32 levelXP = (uint32)(totalLevelXP * newXPpct);
                player->SetUInt32Value(PLAYER_XP, levelXP);
            }
        }
    }

    Unit* HardcoreModule::GetKiller(Player* player) const
    {
        Unit* killer = nullptr;
        if (player)
        {
    #ifdef ENABLE_PLAYERBOTS
            // Ignore bots
            if (!player->isRealPlayer())
                return nullptr;
    #endif

            const uint32 playerGuid = player->GetObjectGuid().GetCounter();
            if (m_lastPlayerDeaths.find(playerGuid) != m_lastPlayerDeaths.end())
            {
                const ObjectGuid& killerGuid = m_lastPlayerDeaths.at(playerGuid);
                return sObjectAccessor.GetUnit(*player, killerGuid);
            }
        }

        return killer;
    }

    void HardcoreModule::SetKiller(Player* player, Unit* killer)
    {
        if (player)
        {
    #ifdef ENABLE_PLAYERBOTS
            // Ignore bots
            if (!player->isRealPlayer())
                return;
    #endif

            const uint32 playerGuid = player->GetObjectGuid().GetCounter();
            const ObjectGuid killerGuid = killer ? killer->GetObjectGuid() : ObjectGuid();
            m_lastPlayerDeaths[playerGuid] = killerGuid;
        }
    }

    bool HardcoreModule::IsDropLootEnabled() const
    {
        return GetConfig()->dropGearPct > 0.0f || 
    #ifdef ENABLE_PLAYERBOTS
               GetConfig()->botDropGearPct > 0.0f ||
               GetConfig()->botDropItemsPct > 0.0f ||
               GetConfig()->botDropMoneyPct > 0.0f ||
    #endif
               GetConfig()->dropItemsPct > 0.0f || 
               GetConfig()->dropMoneyPct > 0.0f;
    }

    /*
    bool ChatHandler::HandleHardcoreCommand(char* args)
    {
        std::string command = args;
        char* cmd = strtok((char*)args, " ");
        char* charname = strtok(NULL, " ");
        if (cmd)
        {
            if (!strcmp(cmd, "reset"))
            {
                sHardcoreModule.RemoveAllLoot();
                sHardcoreModule.RemoveAllGraves();
            }
            else if (!strcmp(cmd, "resetgraves"))
            {
                sHardcoreModule.RemoveAllGraves();
            }
            else if (!strcmp(cmd, "resetloot"))
            {
                sHardcoreModule.RemoveAllLoot();
            }
            else if (!strcmp(cmd, "spawnloot"))
            {
                if (m_session && m_session->GetPlayer())
                {
                    sHardcoreModule.CreateLoot(m_session->GetPlayer(), nullptr);
                }
            }
            else if (!strcmp(cmd, "spawngrave"))
            {
                if (m_session && m_session->GetPlayer())
                {
                    sHardcoreModule.CreateGrave(m_session->GetPlayer());
                }
            }
            else if (!strcmp(cmd, "leveldown"))
            {
                if (m_session && m_session->GetPlayer())
                {
                    sHardcoreModule.LevelDown(m_session->GetPlayer());
                }
            }
        }

        return true;
    }
    */
}
