#include "TwistedTreasures.h"
#include "TwistedMgr.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "LootMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Random.h"
#include <map>
#include <vector>

std::multimap<uint32, uint32> ArmorUpgradeMap;

TwistedTreasuresWorldScript::TwistedTreasuresWorldScript() : WorldScript("TwistedTreasuresWorldScript")
{
}

void TwistedTreasuresWorldScript::OnStartup()
{
    QueryResult result = WorldDatabase.Query(
        "SELECT ItemLevel, Entry FROM item_template WHERE RequiredLevel > 0 AND RequiredSkill = 0 AND (RandomProperty != 0 OR RandomSuffix != 0) ORDER BY ItemLevel asc");

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemLevel = fields[0].Get<uint32>();
            uint32 entry = fields[1].Get<uint32>();
            ArmorUpgradeMap.emplace(itemLevel, entry);
        } while (result->NextRow());

        LOG_INFO("twisted", "Mod-Twisted: Loaded {} items into ArmorUpgradeMap", ArmorUpgradeMap.size());
    }
    else
    {
        LOG_WARN("twisted", "Mod-Twisted: No items found for ArmorUpgradeMap");
    }
}

TwistedTreasuresMiscScript::TwistedTreasuresMiscScript() : MiscScript("TwistedTreasuresMiscScript",
    {
        MISCHOOK_ON_AFTER_LOOT_TEMPLATE_PROCESS,
    })
{
}

void TwistedTreasuresMiscScript::OnAfterLootTemplateProcess(Loot* loot, LootTemplate const* /*tab*/, LootStore const& /*store*/, Player* lootOwner, bool /*personal*/, bool /*noEmptyError*/, uint16 /*lootMode*/)
{
    if (!sTwistedMgr->GetTreasureFindEnabled())
        return;

    if (!lootOwner)
        return;

    const uint32 numRewards = sTwistedMgr->RollForTreasureFind(lootOwner);
    if (numRewards == 0)
        return;

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

        const LootStoreItem selectedItem(selectedEntry, 0, 100.0f, false, LOOT_MODE_DEFAULT, 0, 1, 1);
        loot->AddItem(selectedItem);
        awardedEntries.push_back(selectedEntry);
    }
}
