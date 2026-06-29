#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TwistedMgr.h"

#include "TwistedConfig.h"
#include "TwistedSpells.h"
#include "TwistedPlayer.h"
#include "TwistedTreasures.h"
#include <MapMgr.h>
#include "Creature.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "Mail.h"
#include "ObjectMgr.h"
#include "QuestDef.h"
#include "Random.h"

TwistedMgr* TwistedMgr::Get()
{
    static TwistedMgr instance;
    return &instance;
}

void TwistedMgr::PlayerInitialize(Player* player)
{
    TwistedPlayerData* pData = GetOrFindPlayerData(player->GetGUID());
    if (!pData)
        return;

    QueryResult result = CharacterDatabase.Query(
        "SELECT guid, rewardxp FROM twisted_playerdata WHERE guid = {}",
        player->GetGUID().GetCounter());

    if (result)
    {
        Field* fields = result->Fetch();
        pData->RewardXP = fields[1].Get<uint32>();
    }
    else
    {
        pData->RewardXP = StartingRewardXP;
    }
}

void TwistedMgr::PlayerCleanup(ObjectGuid guid)
{
    auto entry = PlayerData.find(guid);
    if (entry != PlayerData.end())
    {
        CharacterDatabase.Execute(
            "REPLACE INTO twisted_playerdata (guid, rewardxp) VALUES ({}, {})",
            guid.GetCounter(),
            entry->second.RewardXP);
    }

    PlayerData.erase(guid);
}

void TwistedMgr::ModifyTreasureFind(ObjectGuid guid, const int32& Points)
{
    if (TwistedPlayerData* pData = GetOrFindPlayerData(guid))
    {
        pData->TreasureFindPoints += Points;
    }
}

int32 TwistedMgr::GetTreasureFindValue(ObjectGuid guid)
{
    if (const TwistedPlayerData* pData = GetOrFindPlayerData(guid))
    {
        return pData->TreasureFindPoints;
    }

    return 0;
}

const ItemImbueTierData* TwistedMgr::GetItemImbueTier(uint32 ItemId, int32& outIndex)
{
    outIndex = 0;
    for (const ItemImbueTierData& tData : EnchantmentTiers)
    {
        for (const uint32& SourceItem : tData.ItemIds)
        {
            if (SourceItem == ItemId)
            {
                return &tData;
            }
        }
        ++outIndex;
    }

    outIndex = -1;
    return nullptr;
}

const ItemImbueTierData* TwistedMgr::GetItemImbueTier(int32 Index)
{
    if (Index > -1 && Index < EnchantmentTiers.size())
    {
        return &EnchantmentTiers[Index];
    }

    return nullptr;
}

void TwistedMgr::TryGrantReward(Player* player, TwistedPlayerData* pData)
{
    while (pData->RewardXP >= RewardGrantThreshold)
    {
        pData->RewardXP -= RewardGrantThreshold;

        const RewardDefinition* FoundDef = nullptr;
        for (const RewardDefinition& Def : RewardDefinitions)
        {
            if (player->GetLevel() >= Def.MinLevel && player->GetLevel() <= Def.MaxLevel)
            {
                FoundDef = &Def;
                break;
            }
        }

        if (!FoundDef)
        {
            LOG_WARN("twisted", "Mod-Twisted: No RewardDefinition found for player {} at level {} — skipping grant.",
                player->GetName().c_str(), player->GetLevel());
            continue;
        }

        ItemPosCountVec dest;
        const InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, FoundDef->EntryId, 1);
        if (msg == EQUIP_ERR_OK)
        {
            Item* item = player->StoreNewItem(dest, FoundDef->EntryId, true);
            if (item)
                player->SendNewItem(item, 1, true, false, false, true);
        }
        else
        {
            Item* item = Item::CreateItem(FoundDef->EntryId, 1, player);
            if (item)
            {
                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                item->SaveToDB(trans);
                MailDraft("A Gift for You!", "Your inventory was full during your last reward. You can reclaim it here.")
                    .AddItem(item)
                    .SendMailTo(trans, MailReceiver(player), MailSender(MAIL_NORMAL, player->GetGUID().GetCounter()));
                CharacterDatabase.CommitTransaction(trans);
            }
        }
    }
}

void TwistedMgr::OnPlayerCreatureKill(Player* player, Creature* killed)
{
    TwistedPlayerData* pData = GetOrFindPlayerData(player->GetGUID());
    if (!pData)
        return;

    int32 levelDiff = static_cast<int32>(player->GetLevel()) - static_cast<int32>(killed->GetLevel());
    if (levelDiff < 0)
        levelDiff = -levelDiff;

    uint32 gainedXP = 0;
    if (levelDiff <= 5)
    {
        gainedXP = OnCreatureKillInLevelRangeXP;
        pData->RewardXP += gainedXP;
    }
    else
    {
        gainedXP = OnCreatureKillAnyLevelXP;
        pData->RewardXP += gainedXP;
    }

    const uint32 remaining = RewardGrantThreshold > 0
        ? RewardGrantThreshold - (pData->RewardXP % RewardGrantThreshold)
        : 0;
    LOG_INFO("twisted", "Player {} gained {} RewardXP from creature kill. Total: {}, remaining to next reward: {}",
        player->GetName().c_str(), gainedXP, pData->RewardXP, remaining);

    TryGrantReward(player, pData);
}

void TwistedMgr::OnPlayerCompleteQuest(Player* player, Quest const* /*quest*/)
{
    TwistedPlayerData* pData = GetOrFindPlayerData(player->GetGUID());
    if (!pData)
        return;

    pData->RewardXP += OnQuestTurnInXp;

    const uint32 remaining = RewardGrantThreshold > 0
        ? RewardGrantThreshold - (pData->RewardXP % RewardGrantThreshold)
        : 0;
    LOG_INFO("twisted", "Player {} gained {} RewardXP from quest turn-in. Total: {}, remaining to next reward: {}",
        player->GetName().c_str(), OnQuestTurnInXp, pData->RewardXP, remaining);

    TryGrantReward(player, pData);
}

void TwistedMgr::OnPlayerLevelChanged(Player* player, uint8 oldLevel)
{
    TwistedPlayerData* pData = GetOrFindPlayerData(player->GetGUID());
    if (!pData)
        return;

    const uint8 newLevel = player->GetLevel();
    if (newLevel > oldLevel)
    {
        const uint32 gainedXP = LevelUpXP * (newLevel - oldLevel);
        pData->RewardXP += gainedXP;

        const uint32 remaining = RewardGrantThreshold > 0
            ? RewardGrantThreshold - (pData->RewardXP % RewardGrantThreshold)
            : 0;
        LOG_INFO("twisted", "Player {} gained {} RewardXP from level-up. Total: {}, remaining to next reward: {}",
            player->GetName().c_str(), gainedXP, pData->RewardXP, remaining);

        TryGrantReward(player, pData);
    }
}

uint32 TwistedMgr::RollForTreasureFind(Player* player)
{
    const uint8 playerLevel = player->GetLevel();
    const int32 treasureFindValue = GetTreasureFindValue(player->GetGUID());

    uint32 successfulRolls = 0;
    int32 remainingValue = treasureFindValue;

    while (remainingValue > 0)
    {
        const int32 rollValue = std::min(remainingValue, 100);
        remainingValue -= rollValue;

        float levelMultiplier = 1.0f;
        if (playerLevel <= 10)
        {
            levelMultiplier = 8.0f - (playerLevel - 1) * 0.2f;
        }
        else if (playerLevel <= 20)
        {
            levelMultiplier = 6.0f - (playerLevel - 11) * 0.3f;
        }
        else if (playerLevel <= 30)
        {
            levelMultiplier = 3.0f - (playerLevel - 21) * 0.15f;
        }
        else if (playerLevel <= 40)
        {
            levelMultiplier = 1.5f - (playerLevel - 31) * 0.05f;
        }
        else
        {
            levelMultiplier = 1.0f;
        }

        const float scaledValue = rollValue * levelMultiplier;
        const float effectiveTreasureFind = scaledValue * 0.25f;

        if (urand(0, 100) < effectiveTreasureFind)
        {
            successfulRolls++;
        }
    }

    return successfulRolls;
}

TwistedPlayerData* TwistedMgr::GetOrFindPlayerData(ObjectGuid guid)
{
    auto entry = PlayerData.find(guid);
    if (entry != PlayerData.end())
    {
        return &entry->second;
    }

    auto [Pair, Success] = PlayerData.emplace(guid, TwistedPlayerData());
    return &Pair->second;
}

void AddTwistedScripts()
{
    new TwistedConfig();
    new TwistedPlayer();
    new TwistedTreasuresWorldScript();
    new TwistedTreasuresMiscScript();

    AddTwistedSpellScripts();
}
