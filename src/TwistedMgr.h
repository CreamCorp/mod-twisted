#ifndef TWISTED_MGR_H
#define TWISTED_MGR_H

#include "Position.h"
#include "ObjectGuid.h"
#include <map>
#include <vector>

class Creature;
class Player;
class Map;
class Quest;

// Structure for holding extra data for a player
struct TwistedPlayerData
{
public:
    TwistedPlayerData()
        : TreasureFindPoints(0)
        , RewardXP(0)
    {
    }

    int32  TreasureFindPoints = 0;
    uint32 RewardXP           = 0;
};

// Structure for defining a level-gated item reward
struct RewardDefinition
{
    uint32 EntryId   = 0;
    uint32 MinLevel  = 1;
    uint32 MaxLevel  = 80;
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

    void PlayerInitialize(Player* player);
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

    void OnPlayerCreatureKill(Player* player, Creature* killed);
    void OnPlayerCompleteQuest(Player* player, Quest const* quest);
    void OnPlayerLevelChanged(Player* player, uint8 oldLevel);

private:
    TwistedPlayerData* GetOrFindPlayerData(ObjectGuid guid);
    void TryGrantReward(Player* player, TwistedPlayerData* pData);

    // Player Data
    std::map<ObjectGuid, TwistedPlayerData> PlayerData;

    // Treasure Find
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

    // Rewards
    uint32 NumRewardDefs                    = 0;
    std::vector<RewardDefinition> RewardDefinitions;
    uint32 StartingRewardXP                 = 0;
    uint32 RewardGrantThreshold             = 10000;
    uint32 LevelUpXP                        = 6000;
    uint32 OnCreatureKillInLevelRangeXP     = 100;
    uint32 OnCreatureKillAnyLevelXP         = 25;
    uint32 OnQuestTurnInXp                  = 2350;
};

#define sTwistedMgr TwistedMgr::Get()

#endif // TWISTED_MGR_H
