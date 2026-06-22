#include "TwistedConfig.h"
#include "TwistedMgr.h"
#include <cstdio>
#include <string>

TwistedConfig::TwistedConfig() : WorldScript("TwistedConfig")
{
}

#define CLR_STREAM(oss) oss.str(""); oss.clear();
#define GET_CONF_VALUE(data_type, member_var, conf_str, default_val) \
    sTwistedMgr->member_var = sConfigMgr->GetOption<data_type>(conf_str, default_val);

// Graciously lifted from DungeonScale
template<class T>
void ParseConfStringList(std::vector<T>& refList, std::string inputString)
{
    std::string delimitedValue;
    std::stringstream intStringStream;
    refList.clear();

    intStringStream.str(inputString);
    while (std::getline(intStringStream, delimitedValue, ','))
    {
        std::string valueOne;
        std::stringstream intStream(delimitedValue);
        intStream >> valueOne;
        auto intValue = atoi(valueOne.c_str());
        refList.push_back(intValue);
    }
}

void TwistedConfig::OnBeforeConfigLoad(bool /*reload*/)
{
    LoadConfig();
}

void TwistedConfig::OnShutdown()
{
    sTwistedMgr->EnchantmentTiers.clear();
}

void TwistedConfig::LoadConfig()
{
    GET_CONF_VALUE(bool, TreasureFindEnabled, "Twisted.Treasures.Enable", false);
    LoadImbueEnchantments();
}

void TwistedConfig::LoadImbueEnchantments()
{
    GET_CONF_VALUE(bool, ItemImbueEnabled, "Twisted.ItemImbue.Enable", false);

    GET_CONF_VALUE(uint32, NumItemImbueTiers, "Twisted.ItemImbue.NumTiers", 1);
    GET_CONF_VALUE(float, ArmorUniqueChance, "Twisted.ItemImbue.ArmorUniqueChance", 0.1f);
    GET_CONF_VALUE(float, WeaponUniqueChance, "Twisted.ItemImbue.WeaponUniqueChance", 0.1f);
    GET_CONF_VALUE(float, TierUpgradeChance, "Twisted.ItemImbue.TierUpgradeChance", 0.1f);
    ParseConfStringList<uint32>(sTwistedMgr->ArmorUniqueEnchantIds, "Twisted.ItemImbue.ArmorUniqueEnchantIds");
    ParseConfStringList<uint32>(sTwistedMgr->WeaponUniqueEnchantIds, "Twisted.ItemImbue.WeaponUniqueEnchantIds");

    LoadImbueEnchantmentTiers();
}

void TwistedConfig::LoadImbueEnchantmentTiers()
{
    std::ostringstream sb;
    const std::string BaseConfStr = "Twisted.ItemImbue.Tier";
    for (int i = 0; i < sTwistedMgr->NumItemImbueTiers; ++i)
    {
        const uint32 Tier = i + 1;
        ItemImbueTierData TierData;

        CLR_STREAM(sb)
        sb << BaseConfStr << Tier << ".ItemIds";
        ParseConfStringList<uint32>(TierData.ItemIds, sConfigMgr->GetOption<std::string>(sb.str(), ""));
        if (TierData.ItemIds.size() <= 0)
        {
            LOG_ERROR("Mod-Twisted conf string [%s] had no source items for Tier %d!", sb.str(), Tier);
        }

        CLR_STREAM(sb)
        sb << BaseConfStr << Tier << ".EnchantIds";
        ParseConfStringList<uint32>(TierData.EnchantIds, sConfigMgr->GetOption<std::string>(sb.str(), ""));
        if (TierData.EnchantIds.size() <= 0)
        {
            LOG_ERROR("Mod-Twisted conf string [%s] had no enchant ids for Tier %d!", sb.str(), Tier);
        }
        sTwistedMgr->EnchantmentTiers.push_back(TierData);
    };
}
