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
    sTwistedMgr->RewardDefinitions.clear();
}

void TwistedConfig::LoadConfig()
{
    GET_CONF_VALUE(bool, TreasureFindEnabled, "Twisted.Treasures.Enable", false);
    LoadImbueEnchantments();
    LoadRewards();
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

void TwistedConfig::LoadRewards()
{
    GET_CONF_VALUE(uint32, NumRewardDefs,               "Twisted.Rewards.NumDefs",                      0);
    GET_CONF_VALUE(uint32, StartingRewardXP,            "Twisted.Rewards.StartingRewardXP",             0);
    GET_CONF_VALUE(uint32, RewardGrantThreshold,        "Twisted.Rewards.RewardGrantThreshold",         10000);
    GET_CONF_VALUE(uint32, LevelUpXP,                   "Twisted.Rewards.LevelUpXP",                    6000);
    GET_CONF_VALUE(uint32, OnCreatureKillInLevelRangeXP,"Twisted.Rewards.OnCreatureKillInLevelRangeXP", 100);
    GET_CONF_VALUE(uint32, OnCreatureKillAnyLevelXP,    "Twisted.Rewards.OnCreatureKillAnyLevelXP",     25);
    GET_CONF_VALUE(uint32, OnQuestTurnInXp,             "Twisted.Rewards.OnQuestTurnInXp",              2350);

    LoadRewardDefinitions();
}

void TwistedConfig::LoadRewardDefinitions()
{
    std::ostringstream sb;
    const std::string BaseConfStr = "Twisted.Rewards.Def";
    sTwistedMgr->RewardDefinitions.clear();
    for (uint32 i = 0; i < sTwistedMgr->NumRewardDefs; ++i)
    {
        const uint32 Def = i + 1;
        RewardDefinition DefData;

        CLR_STREAM(sb)
        sb << BaseConfStr << Def << ".RewardItemId";
        DefData.EntryId = sConfigMgr->GetOption<uint32>(sb.str(), 0);
        if (DefData.EntryId == 0)
        {
            LOG_ERROR("twisted", "Conf string [{}] had no item entry for Def {}!", sb.str().c_str(), Def);
        }
        
        sLog->SetLogLevel("Twisted", LogLevel::LOG_LEVEL_DISABLED);

        CLR_STREAM(sb)
        sb << BaseConfStr << Def << ".MinLevel";
        DefData.MinLevel = sConfigMgr->GetOption<uint32>(sb.str(), 1);

        CLR_STREAM(sb)
        sb << BaseConfStr << Def << ".MaxLevel";
        DefData.MaxLevel = sConfigMgr->GetOption<uint32>(sb.str(), 80);

        sTwistedMgr->RewardDefinitions.push_back(DefData);
    }
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
            LOG_ERROR("twisted", "Conf string [{}] had no source items for Tier {}!", sb.str(), Tier);
        }

        CLR_STREAM(sb)
        sb << BaseConfStr << Tier << ".EnchantIds";
        ParseConfStringList<uint32>(TierData.EnchantIds, sConfigMgr->GetOption<std::string>(sb.str(), ""));
        if (TierData.EnchantIds.size() <= 0)
        {
            LOG_ERROR("twisted", "Conf string [{}] had no enchant ids for Tier {}!", sb.str(), Tier);
        }
        sTwistedMgr->EnchantmentTiers.push_back(TierData);
    };
}
