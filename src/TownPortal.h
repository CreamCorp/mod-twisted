#ifndef TOWN_PORTAL_H
#define TOWN_PORTAL_H

#include "ScriptMgr.h"
#include "SpellScript.h"

class Player;
class GameObject;

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

class TownPortalSpell : public SpellScript
{
public:
    PrepareSpellScript(TownPortalSpell);

    void HandleCast();
    void Register() override;
};

#endif // TOWN_PORTAL_H
