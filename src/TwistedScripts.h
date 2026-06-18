#ifndef TWISTED_SCRIPTS_H
#define TWISTED_SCRIPTS_H

namespace twisted
{
    
    enum SpellIds
    {
        PortalFx      = 36400,
    };

    struct FieldBind
    {
        uint32 MapId;
        uint16 AreaId;
        float X;
        float Y;
        float Z;
        float O;
    };

    class static_data
    {
        std::map<ObjectGuid, FieldBind> PlayerFieldBinds;

        void SetFieldbind(ObjectGuid guid, WorldLocation const& loc, uint32 areaId);
    };

    class TwistedPlayer : public PlayerScript
    {
        TwistedPlayer();

        void OnPlayerLogout(Player* player) override;
    };

    class TownPortal : public GameObjectScript
    {
    public:
        TownPortal();

        bool OnGossipHello(Player* player, GameObject* go) override;
    };

    class FieldPortal : public GameObjectScript
    {
    public:
        FieldPortal();

        bool OnGossipHello(Player* player, GameObject* go) override;
    };
}

#endif // TWISTED_SCRIPTS_H