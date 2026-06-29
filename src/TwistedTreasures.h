#ifndef TWISTED_TREASURES_H
#define TWISTED_TREASURES_H

#include "ScriptMgr.h"
#include <map>
#include <vector>

class TwistedTreasuresWorldScript : public WorldScript
{
public:
    TwistedTreasuresWorldScript();

    void OnStartup() override;
};

class TwistedTreasuresMiscScript : public MiscScript
{
public:
    TwistedTreasuresMiscScript();

    void OnAfterLootTemplateProcess(Loot* loot, LootTemplate const* tab, LootStore const& store, Player* lootOwner, bool personal, bool noEmptyError, uint16 lootMode) override;
};

#endif // TWISTED_TREASURES_H
