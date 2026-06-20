#include "TwistedMgr.h"
#include "TownPortal.h"
#include "GameObjectAI.h"
#include "Player.h"

TownPortal::TownPortal() : GameObjectScript("TownPortal")
{

}

bool TownPortal::OnGossipHello(Player* player, GameObject* go)
{
    WorldLocation homeLoc = player->GetWorldLocation();
    uint32 areaId = player->GetAreaId();
    sTwistedMgr->SetFieldbind(player->GetGUID(), homeLoc, areaId);

    player->CastCustomSpell(SpellIds::PortalFx, CustomSpellValues(), player, TRIGGERED_NONE);
    player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, 0);

    return true;
}

FieldPortal::FieldPortal() : GameObjectScript("FieldPortal")
{

}

bool FieldPortal::OnGossipHello(Player* player, GameObject* go)
{
    auto iter = sTwistedMgr->GetPlayerFieldBinds().find(player->GetGUID());
    if (iter == sTwistedMgr->GetPlayerFieldBinds().end())
    {
        return false;
    }

    const FieldBind& bind = iter->second;

    player->CastCustomSpell(SpellIds::PortalFx, CustomSpellValues(), player, TRIGGERED_NONE);
    player->TeleportTo(bind.MapId, bind.X, bind.Y, bind.Z, bind.O);

    return true;
}
