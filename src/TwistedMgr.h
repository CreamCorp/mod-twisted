#ifndef TWISTED_MGR_H
#define TWISTED_MGR_H

#include "Position.h"
#include "ObjectGuid.h"
#include <map>
#include <vector>

class GameObject;
class GameObjectTemplate;
class Player;
class Map;

enum SpellIds
{
    PortalFx = 36400,
};

struct FieldBind
{
public:
    FieldBind()
        : MapId(0), AreaId(0), X(0.0f), Y(0.0f), Z(0.0f), O(0.0f)
    {
    }

    uint32 MapId = 0;
    uint16 AreaId = 0;
    float X, Y, Z, O = 0.0f;
};

// Structure for holding extra data for a player
struct TwistedPlayerData
{
public:
    TwistedPlayerData()
        : TreasureFindPoints(0)
    {
    }

    int32 TreasureFindPoints = 0;
};

// Structure for defining the data of an item imbue tier
struct ItemImbueTierData
{
public:
    ItemImbueTierData() {}

    std::vector<uint32> ItemIds;
    std::vector<uint32> EnchantIds;
};

class TwistedMgr
{
    friend class TwistedConfig;

public:
    static TwistedMgr* Get();

    void PlayerCleanup(ObjectGuid guid);

    bool GetTreasureFindEnabled() const { return TreasureFindEnabled; }
    void ModifyTreasureFind(ObjectGuid guid, const int32& Points);

    bool GetItemImbueEnabled() const { return ItemImbueEnabled; }
    uint32 GetNumItemImbueTiers() const { return NumItemImbueTiers; }
    float GetTierUpgradeChance() const { return TierUpgradeChance; }
    float GetArmorUniqueChance() const { return ArmorUniqueChance; }
    float GetWeaponUniqueChance() const { return WeaponUniqueChance; }
    const std::vector<uint32>& GetArmorUniqueEnchantIds() const { return ArmorUniqueEnchantIds; }
    const std::vector<uint32>& GetWeaponUniqueEnchantIds() const { return WeaponUniqueEnchantIds; }

    const ItemImbueTierData* GetItemImbueTier(uint32 ItemId, int32& outIndex);
    const ItemImbueTierData* GetItemImbueTier(int32 Index);

private:
    TwistedPlayerData* GetOrFindPlayerData(ObjectGuid guid);
    GameObject* SpawnGameObject(ObjectGuid::LowType guidlow, GameObjectTemplate const* goinfo, Map* map, const Position& pos, const float orientation, const uint32 phasemask);

    // Player Data
    std::map<ObjectGuid, TwistedPlayerData> PlayerData;

    bool TreasureFindEnabled = true;

    // Item Imbues
    bool ItemImbueEnabled = true;
    uint32 NumItemImbueTiers = 1;
    std::vector<ItemImbueTierData> EnchantmentTiers;
    float ArmorUniqueChance = 0.1f;
    float WeaponUniqueChance = 0.1f;
    float TierUpgradeChance = 0.1f;
    std::vector<uint32> ArmorUniqueEnchantIds;
    std::vector<uint32> WeaponUniqueEnchantIds;
};

#define sTwistedMgr TwistedMgr::Get()

#endif // TWISTED_MGR_H
