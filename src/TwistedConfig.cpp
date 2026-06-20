#include "TwistedConfig.h"
#include "TwistedMgr.h"

TwistedConfig::TwistedConfig() : WorldScript("TwistedConfig")
{

}

// Graciously lifted from DungeonScale
std::vector<uint32> ParseIntsFromString(std::string inputString) // Used when parsing strings that have comma delimited ints
{
    std::string delimitedValue;
    std::stringstream intStringStream;
    std::vector<uint32> returnIntList;

    intStringStream.str(inputString);
    while (std::getline(intStringStream, delimitedValue, ',')) // Process each int in the string, delimited by the comma - ","
    {
        std::string valueOne;
        std::stringstream intStream(delimitedValue);
        intStream >> valueOne;
        auto intValue = atoi(valueOne.c_str());
        returnIntList.push_back(intValue);
    }

    return returnIntList;
}

void TwistedConfig::OnBeforeConfigLoad(bool /*reload*/)
{
    LoadConfig();
}

void TwistedConfig::LoadConfig()
{
    sTwistedMgr->TownPortalObject = sConfigMgr->GetOption<uint32>("Twisted.TownPortalId", 0);
    sTwistedMgr->ReturnPortalObject = sConfigMgr->GetOption<uint32>("Twisted.ReturnPortalId", 0);
    sTwistedMgr->TownPortalDurationSeconds = sConfigMgr->GetOption<uint32>("Twisted.TownPortalSeconds", 0);
    sTwistedMgr->ReturnPortalDurationSeconds = sConfigMgr->GetOption<uint32>("Twisted.ReturnPortalSeconds", 0);
    sTwistedMgr->ItemImbueEnchantments = ParseIntsFromString(sConfigMgr->GetOption<std::string>("Twisted.ImbueItem.Tier1.EnchantIds", ""));
}
