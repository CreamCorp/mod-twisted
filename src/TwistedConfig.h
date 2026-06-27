#ifndef TWISTED_CONFIG_H
#define TWISTED_CONFIG_H

#include "ScriptMgr.h"

class TwistedConfig : public WorldScript
{
public:
    TwistedConfig();

    void OnBeforeConfigLoad(bool /*reload*/) override;
    void OnShutdown() override;

private:
    void LoadConfig();
    void LoadImbueEnchantments();
    void LoadImbueEnchantmentTiers();
    void LoadRewards();
    void LoadRewardDefinitions();
};

#endif // TWISTED_CONFIG_H
