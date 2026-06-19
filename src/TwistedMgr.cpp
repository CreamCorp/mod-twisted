#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TwistedMgr.h"

#include "TownPortal.h"
#include "TwistedSpells.h"
#include "TwistedPlayer.h"

TwistedMgr* TwistedMgr::Get()
{
    static TwistedMgr instance;
    return &instance;
}

void TwistedMgr::SetFieldbind(ObjectGuid guid, WorldLocation const& loc, uint32 areaId)
{
    FieldBind bind;
    loc.GetPosition(bind.X, bind.Y, bind.Z, bind.O);
    bind.MapId = loc.GetMapId();
    bind.AreaId = areaId;

    PlayerFieldBinds.emplace(guid, bind);
}

void AddTwistedScripts()
{
    new TwistedPlayer();
    new TownPortal();
    new FieldPortal();
}
