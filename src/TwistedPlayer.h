#ifndef TWISTED_PLAYER_H
#define TWISTED_PLAYER_H

#include "ScriptMgr.h"

class Player;

class TwistedPlayer : public PlayerScript
{
public:
    TwistedPlayer();

    void OnPlayerLogout(Player* player) override;
};

#endif // TWISTED_PLAYER_H
