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

struct TwistedPlayerData
{
public:
    TwistedPlayerData()
        : TreasureFindPoints(0)
    {
    }

    int32 TreasureFindPoints = 0;
};

class TwistedMgr
{
    friend class TwistedConfig;

public:
    static TwistedMgr* Get();

    void PlayerCleanup(ObjectGuid guid);
    void SetFieldbind(ObjectGuid guid, WorldLocation const& loc, uint32 areaId);
    void ModifyTreasureFind(ObjectGuid guid, const int32& Points);
    GameObject* SpawnGameObject(ObjectGuid::LowType guidlow, GameObjectTemplate const* goinfo, Map* map, const Position& pos, const float orientation, const uint32 phasemask);
    GameObject* ObtainTownPortal(GameObjectTemplate const* object, Player* caster);
    GameObject* ObtainTownPortalObject(GameObjectTemplate const* object, Player* caster);
    GameObject* ObtainReturnPortalObject(GameObjectTemplate const* object, Player* caster);

    std::map<ObjectGuid, FieldBind>& GetPlayerFieldBinds() { return PlayerFieldBinds; }
    std::map<ObjectGuid, ObjectGuid>& GetPlayerPortals() { return PlayerPortals; }
    std::map<uint32, ObjectGuid>& GetAreaPortals() { return AreaPortals; }

    const std::vector<uint32>& GetItemImbueEnchantments() const { return ItemImbueEnchantments; }

private:
    TwistedPlayerData* GetOrFindPlayerData(ObjectGuid guid);

    std::map<ObjectGuid, TwistedPlayerData> PlayerData;
    std::map<ObjectGuid, FieldBind> PlayerFieldBinds;
    std::map<ObjectGuid, ObjectGuid> PlayerPortals;
    std::map<uint32, ObjectGuid> AreaPortals;
    std::vector<uint32> ItemImbueEnchantments;

    uint32 TownPortalObject = 0;
    uint32 ReturnPortalObject = 0;
    uint32 TownPortalDurationSeconds = 0;
    uint32 ReturnPortalDurationSeconds = 0;

};

#define sTwistedMgr TwistedMgr::Get()

#endif // TWISTED_MGR_H
