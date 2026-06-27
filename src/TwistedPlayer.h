#ifndef TWISTED_PLAYER_H
#define TWISTED_PLAYER_H

#include "ScriptMgr.h"

class Creature;
class Player;
class Quest;

class TwistedPlayer : public PlayerScript
{
public:
    TwistedPlayer();

    void OnPlayerLogin(Player* player) override;
    void OnPlayerLogout(Player* player) override;
    void OnPlayerCreatureKill(Player* killer, Creature* killed) override;
    void OnPlayerCreatureKilledByPet(Player* petOwner, Creature* killed) override;
    void OnPlayerCompleteQuest(Player* player, Quest const* quest) override;
    void OnPlayerLevelChanged(Player* player, uint8 oldLevel) override;
};

#endif // TWISTED_PLAYER_H
