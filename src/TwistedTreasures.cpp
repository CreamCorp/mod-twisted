#include "TwistedTreasures.h"
#include "TwistedMgr.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "LootMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Random.h"
#include "WorldScript.h"
#include "MiscScript.h"
#include <map>
#include <vector>

std::multimap<uint32, uint32> ArmorUpgradeMap;

class TwistedTreasuresWorldScript : public WorldScript
{
public:
    TwistedTreasuresWorldScript() : WorldScript("TwistedTreasuresWorldScript")
    {
    }

    void ProcessResults(QueryResult& result)
    {
        if (result)
        {
            do
            {
                const Field* fields = result->Fetch();
                const uint32 itemLevel = fields[0].Get<uint32>();
                const uint32 entry = fields[1].Get<uint32>();
                ArmorUpgradeMap.emplace(itemLevel, entry);
            } while (result->NextRow());

            LOG_INFO("twisted", "Mod-Twisted: Loaded {} items into ArmorUpgradeMap", ArmorUpgradeMap.size());
        }
    }

    void LoadRandomItems()
    {
        // All items that have random enchants or properties, most of our loot pools
        QueryResult result = WorldDatabase.Query(
            "SELECT ItemLevel, Entry FROM item_template WHERE RequiredLevel > 0 AND RequiredSkill = 0 AND (RandomProperty != 0 OR RandomSuffix != 0) ORDER BY ItemLevel asc");

        ProcessResults(result);
    }

    void LoadEarlyGameItems()
    {
        // Filling out ilvls 10 to 20 mostly
        QueryResult result = WorldDatabase.Query(
            "SELECT ItemLevel, Entry FROM item_template AS i \
                WHERE ItemLevel > 2 AND ItemLevel < 20 AND Quality = 2 AND (Class = 2 OR Class = 4) \
                AND Name NOT LIKE '%test%' AND Name NOT LIKE '%rank%' AND Name NOT LIKE '%old%'  \
                AND NOT EXISTS( \
                    SELECT 1 \
                    FROM quest_template AS q \
                    WHERE i.Entry IN( \
                        q.RewardItem1, \
                        q.RewardItem2, \
                        q.RewardItem3, \
                        q.RewardItem4 \
                    ) \
                ) ORDER BY ItemLevel Asc; ");

        ProcessResults(result);
    }

    void LoadLowChanceWorldDrops()
    {
        // Smattering of blues and 18 epics across ilvl 20 to 60 from world drops with low chances. Fun finds.
        // We keep this capped at ilevel 60 because then we start getting into tier sets and pvp gear.
        QueryResult result = WorldDatabase.Query(
            "SELECT ItemLevel, Entry, Name from acore_world.item_template AS i \
                WHERE ItemLevel > 10 AND ItemLevel < 60 \
                AND (Quality = 3 OR Quality = 4) \
                AND (Class = 2 OR Class = 4) \
                AND NOT EXISTS( \
                    SELECT 1 \
                    FROM quest_template AS q \
                    WHERE i.Entry IN( \
                        q.RewardItem1, \
                        q.RewardItem2, \
                        q.RewardItem3, \
                        q.RewardItem4 \
                    ) \
                ) AND EXISTS( \
                    SELECT 1 \
                    FROM creature_loot_template AS c \
                    WHERE c.Entry = i.Entry AND Chance < 5 \
                ) ORDER BY ItemLevel Asc;");

        ProcessResults(result);
    }

    void OnStartup() override
    {
        LoadEarlyGameItems();
        LoadRandomItems();
        LoadLowChanceWorldDrops();
        // With the above, we fill out a bunch of neat stuff to find on your journey to level 60.
        // Items with random properties will still continue to 80, but we need to find more appropriate queries
        // to fill out more special finds from 60 to 80 that aren't just raid loot.. for another time.
    }
};

class TwistedTreasuresMiscScript : public MiscScript
{
public:
    TwistedTreasuresMiscScript() : MiscScript("TwistedTreasuresMiscScript",
    {
        MISCHOOK_ON_AFTER_LOOT_TEMPLATE_PROCESS,
    })
    {
    }

    void OnAfterLootTemplateProcess(Loot* loot, LootTemplate const* /*tab*/, LootStore const& /*store*/, Player* lootOwner, bool /*personal*/, bool /*noEmptyError*/, uint16 /*lootMode*/) override
    {
        if (!sTwistedMgr->GetTreasureFindEnabled())
            return;

        if (!lootOwner)
            return;

        const uint32 numRewards = sTwistedMgr->RollForTreasureFind(lootOwner);
        if (numRewards == 0)
            return;

        LOG_INFO("twisted", "TreasureFind: Player {} to gain {} rewards.",
            lootOwner->GetName().c_str(), numRewards);

        uint32 totalItemLevel = 0;
        uint32 equippedCount = 0;

        for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
        {
            Item* item = lootOwner->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (item)
            {
                ItemTemplate const* proto = item->GetTemplate();
                if (proto)
                {
                    totalItemLevel += proto->ItemLevel;
                    equippedCount++;
                }
            }
        }

        if (equippedCount == 0)
            return;

        const uint32 averageItemLevel = totalItemLevel / equippedCount;
        const uint32 maxItemLevelKey = ArmorUpgradeMap.empty() ? 1u : ArmorUpgradeMap.rbegin()->first;
        const uint32 targetItemLevel = std::min(averageItemLevel + urand(1, 5), maxItemLevelKey);

        std::vector<uint32> candidateEntries;
        auto range = ArmorUpgradeMap.equal_range(targetItemLevel);
        for (auto it = range.first; it != range.second; ++it)
        {
            candidateEntries.push_back(it->second);
        }

        LOG_INFO("twisted", "TreasureFind: - Average ilevel {}, target level {} (max {}), num candidates = {}",
            averageItemLevel, targetItemLevel, maxItemLevelKey, candidateEntries.size());

        if (candidateEntries.empty())
            return;

        std::vector<uint32> awardedEntries;
        for (uint32 i = 0; i < numRewards; ++i)
        {
            std::vector<uint32> availableEntries;
            for (const uint32 entry : candidateEntries)
            {
                bool alreadyAwarded = false;
                for (const uint32 awarded : awardedEntries)
                {
                    if (entry == awarded)
                    {
                        alreadyAwarded = true;
                        break;
                    }
                }
                if (!alreadyAwarded)
                {
                    availableEntries.push_back(entry);
                }
            }

            if (availableEntries.empty())
            {
                const uint32 currencyAmount = 0;
                break;
            }

            const uint32 randomIndex = urand(0, availableEntries.size() - 1);
            const uint32 selectedEntry = availableEntries[randomIndex];

            ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(selectedEntry);
            if (!itemProto)
                continue;

            LOG_INFO("twisted", "TreasureFind: - Adding reward entry {}", selectedEntry);

            const LootStoreItem selectedItem(selectedEntry, 0, 100.0f, false, LOOT_MODE_DEFAULT, 0, 1, 1);
            loot->AddItem(selectedItem);
            awardedEntries.push_back(selectedEntry);
        }
    }

};


void AddTwistedTreasureScripts()
{
    new TwistedTreasuresWorldScript();
    new TwistedTreasuresMiscScript();
}
