#include "TwistedMgr.h"
#include "TwistedPlayer.h"
#include "Creature.h"
#include "Player.h"
#include "QuestDef.h"

TwistedPlayer::TwistedPlayer() : PlayerScript("TwistedPlayer",
    {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_LOGOUT,
        PLAYERHOOK_ON_CREATURE_KILL,
        PLAYERHOOK_ON_CREATURE_KILLED_BY_PET,
        PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST,
        PLAYERHOOK_ON_LEVEL_CHANGED
    })
{

}

void TwistedPlayer::OnPlayerLogin(Player* player)
{
    sTwistedMgr->PlayerInitialize(player);
}

void TwistedPlayer::OnPlayerLogout(Player* player)
{
    sTwistedMgr->PlayerCleanup(player->GetGUID());
}

void TwistedPlayer::OnPlayerCreatureKill(Player* killer, Creature* killed)
{
    sTwistedMgr->OnPlayerCreatureKill(killer, killed);
}

void TwistedPlayer::OnPlayerCreatureKilledByPet(Player* petOwner, Creature* killed)
{
    sTwistedMgr->OnPlayerCreatureKill(petOwner, killed);
}

void TwistedPlayer::OnPlayerCompleteQuest(Player* player, Quest const* quest)
{
    sTwistedMgr->OnPlayerCompleteQuest(player, quest);
}

void TwistedPlayer::OnPlayerLevelChanged(Player* player, uint8 oldLevel)
{
    sTwistedMgr->OnPlayerLevelChanged(player, oldLevel);
}
