#ifndef TOWN_PORTAL_H
#define TOWN_PORTAL_H

#include "ScriptMgr.h"
#include "SpellScript.h"

class Player;
class GameObject;

enum
{
    GO_TOWN_PORTAL      = 250000,
    GO_RETURN_PORTAL    = 250001
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

#endif // TOWN_PORTAL_H
