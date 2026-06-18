#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TwistedScripts.h"

namespace twisted 
{
    static static_data sData;

    void static_data::SetFieldbind(ObjectGuid guid, WorldLocation const& loc, uint32 areaId)
    {
        FieldBind bind;
        loc.GetPosition(bind.X, bind.Y, bind.Z, bind.O);
        bind.MapId = loc.GetMapId();
        bind.AreaId = areaId;

        PlayerFieldBinds.emplace(guid, bind);
    }


    TwistedPlayer::TwistedPlayer() : PlayerScript("TwistedPlayer",
        {
            PLAYERHOOK_ON_LOGOUT
        })
    {

    }

    void TwistedPlayer::OnPlayerLogout(Player* player)
    {
        sData.PlayerFieldBinds.erase(player->GetGUID())
    }

    TownPortal::TownPortal() : GameObjectScript("TownPortal")
    {

    }

    bool TownPortal::OnGossipHello(Player* player, GameObject* go)
    {
        WorldLocation homeLoc = player->GetWorldLocation();
        uint32 areaId = player->GetAreaId();
        sData.SetFieldbind(player->GetGUID(), homeLoc, areaId);

        player->CastCustomSpell(SpellIds.PortalFx, CustomSpellValues(), player, TRIGGERED_NONE);
        player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->m_homebindO);

        return true;
    }

    FieldPortal::FieldPortal() : GameObjectScript("FieldPortal")
    {

    }

    bool FieldPortal::OnGossipHello(Player* player, GameObject* go)
    {
        auto iter = sData.PlayerFieldBinds.find(player->GetGUID());
        if (iter == sData.PlayerFieldBinds.end())
        {
            return false;
        }

        const FieldBind& bind = iter->second;

        player->CastCustomSpell(SpellIds.PortalFx, CustomSpellValues(), player, TRIGGERED_NONE);
        player->TeleportTo(bind.MapId, bind.X, bind.Y, bind.Z, bind.O);

        return true;
    }
}

void AddTwistedScripts()
{
    new twisted::TwistedPlayer();
    new twisted::TownPortal();
    new twisted::FieldPortal();
}
