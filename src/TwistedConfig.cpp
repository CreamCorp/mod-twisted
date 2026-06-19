#include "TwistedConfig.h"
#include "TwistedMgr.h"

TwistedConfig::TwistedConfig() : WorldScript("TwistedConfig")
{

}

void TwistedConfig::OnBeforeConfigLoad(bool /*reload*/)
{
    LoadConfig();
}

void TwistedConfig::LoadConfig()
{
    sTwistedMgr->TownPortalObject = sConfigMgr->GetOption<uint32>("Twisted.TownPortalId", 0);
    sTwistedMgr->ReturnPortalObject = sConfigMgr->GetOption<uint32>("Twisted.ReturnPortalId", 0);
}
