#include "TwistedMgr.h"
#include "TwistedPlayer.h"
#include "Player.h"

TwistedPlayer::TwistedPlayer() : PlayerScript("TwistedPlayer",
    {
        PLAYERHOOK_ON_LOGOUT
    })
{

}

void TwistedPlayer::OnPlayerLogout(Player* player)
{
    sTwistedMgr->PlayerCleanup(player->GetGUID());
}
