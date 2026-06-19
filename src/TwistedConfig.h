#ifndef TWISTED_CONFIG_H
#define TWISTED_CONFIG_H

#include "ScriptMgr.h"

class TwistedConfig : public WorldScript
{
public:
    TwistedConfig();

    void OnBeforeConfigLoad(bool /*reload*/) override;

private:
    void LoadConfig();
};

#endif // TWISTED_CONFIG_H
