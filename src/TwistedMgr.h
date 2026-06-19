#ifndef TWISTED_MGR_H
#define TWISTED_MGR_H

#include "Position.h"
#include "ObjectGuid.h"
#include <map>

enum SpellIds
{
    PortalFx = 36400,
};

struct FieldBind
{
public:
    uint32 MapId;
    uint16 AreaId;
    float X, Y, Z, O;
};

class TwistedMgr
{
public:
    static TwistedMgr* Get();

    void SetFieldbind(ObjectGuid guid, WorldLocation const& loc, uint32 areaId);

    std::map<ObjectGuid, FieldBind> PlayerFieldBinds;
    std::map<ObjectGuid, ObjectGuid> PlayerPortals;
    std::map<uint32, ObjectGuid> AreaPortals;

    uint32 TownPortalObject;
    uint32 ReturnPortalObject;
};

#define sTwistedMgr TwistedMgr::Get()

#endif // TWISTED_MGR_H
