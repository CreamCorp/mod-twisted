#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TwistedMgr.h"

#include "TownPortal.h"
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
    PlayerFieldBinds.erase(guid);
    PlayerPortals.erase(guid);
    PlayerData.erase(guid);
}

void TwistedMgr::SetFieldbind(ObjectGuid guid, WorldLocation const& loc, uint32 areaId)
{
    FieldBind bind;
    loc.GetPosition(bind.X, bind.Y, bind.Z, bind.O);
    bind.MapId = loc.GetMapId();
    bind.AreaId = areaId;

    PlayerFieldBinds.emplace(guid, bind);
}

void TwistedMgr::ModifyTreasureFind(ObjectGuid guid, const int32& Points)
{
    if (TwistedPlayerData* pData = GetOrFindPlayerData(guid))
    {
        pData->TreasureFindPoints += Points;
    }
}

GameObject* TwistedMgr::ObtainTownPortal(GameObjectTemplate const* object, Player* caster)
{
    return (object->entry == TownPortalObject) ? ObtainTownPortalObject(object, caster) : ObtainReturnPortalObject(object, caster);
}

GameObject* TwistedMgr::ObtainTownPortalObject(GameObjectTemplate const* object, Player* caster)
{
    // This is for the town portal in the field to return to an inn

    const ObjectGuid playerid = caster->GetGUID();
    Map* cMap = caster->GetMap();//

    // todo, check how the portal gets the spawn location and use that, it is placed in front of plr

    auto entry = PlayerPortals.find(playerid);
    if (entry != PlayerPortals.end())
    {
        if (GameObject* pGameObject = cMap->GetGameObject(entry->second))
        {
            // new behavior: just set the old gObj to despawn
            pGameObject->SetRespawnTime(1);
        }

        PlayerPortals.erase(playerid);
    }

    if (GameObject* pGameObject = SpawnGameObject(
        cMap->GenerateLowGuid<HighGuid::GameObject>(),
        object,
        cMap,
        caster->GetPosition(),
        caster->GetOrientation(),
        caster->GetPhaseMask()))
    {
        PlayerPortals.emplace(playerid, pGameObject->GetGUID());
        return pGameObject;
    }

    return nullptr;
}

GameObject* TwistedMgr::ObtainReturnPortalObject(GameObjectTemplate const* object, Player* caster)
{
    // These are town portals from an AreaId (inn) to the field

    const uint32 AreaId = caster->GetAreaId();
    Map* cMap = sMapMgr->FindMap(caster->m_homebindMapId, 0);

    auto entry = AreaPortals.find(AreaId);
    if (entry != AreaPortals.end())
    {
        if (GameObject* pGameObject = cMap->GetGameObject(entry->second))
        {
            // refresh the duration of an existing portal if found
            pGameObject->SetRespawnTime(ReturnPortalDurationSeconds); // x minutes to keep a portal open

            return pGameObject;
        }

        AreaPortals.erase(AreaId);
    }

    Position homepos = { caster->m_homebindX, caster->m_homebindY, caster->m_homebindZ };
    if (GameObject* pGameObject = SpawnGameObject(
        cMap->GenerateLowGuid<HighGuid::GameObject>(),
        object,
        cMap,
        homepos,
        0.0f,
        caster->GetPhaseMask()))
    {
        AreaPortals.emplace(AreaId, pGameObject->GetGUID());
        return pGameObject;
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


GameObject* TwistedMgr::SpawnGameObject(ObjectGuid::LowType guidlow, GameObjectTemplate const* goinfo, Map* map, const Position& pos, const float orientation, const uint32 phasemask)
{
    GameObject* pGameObject = new GameObject();
    if (!pGameObject->Create(
        guidlow,
        goinfo->entry,
        map,
        phasemask,
        pos.GetPositionX(),
        pos.GetPositionY(),
        pos.GetPositionZ(),
        orientation,
        G3D::Quat(),
        100,
        GO_STATE_READY))
    {
        delete pGameObject;
        return nullptr;
    }

    const uint32 Duration = goinfo->entry == TownPortalObject ? TownPortalDurationSeconds : ReturnPortalDurationSeconds;
    pGameObject->SetRespawnTime(Duration);

    return pGameObject;
}


void AddTwistedScripts()
{
    new TwistedPlayer();
    new TownPortal();
    new FieldPortal();

    AddTwistedSpellScripts();
}
