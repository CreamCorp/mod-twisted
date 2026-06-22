#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TwistedMgr.h"

#include "TwistedConfig.h"
#include "TwistedSpells.h"
#include "TwistedPlayer.h"
#include <MapMgr.h>

TwistedMgr* TwistedMgr::Get()
{
    static TwistedMgr instance;
    return &instance;
}

void TwistedMgr::PlayerCleanup(ObjectGuid guid)
{
    PlayerData.erase(guid);
}

void TwistedMgr::ModifyTreasureFind(ObjectGuid guid, const int32& Points)
{
    if (TwistedPlayerData* pData = GetOrFindPlayerData(guid))
    {
        pData->TreasureFindPoints += Points;
    }
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

    AddTwistedSpellScripts();
}
