# mod-twisted Agent Guide

Condensed reference for agents working in this module. Covers architecture conventions, patterns used throughout the codebase, and step-by-step workflows for common tasks.

---

## Architecture Overview

| File | Role |
|---|---|
| `TwistedMgr.h/.cpp` | Singleton manager. Owns all feature state, config fields, player data, and game logic. |
| `TwistedConfig.h/.cpp` | `WorldScript` that reads `mod_twisted.conf` on load/reload and writes values into `TwistedMgr`. |
| `TwistedPlayer.h/.cpp` | `PlayerScript` that hooks game events and forwards them to `TwistedMgr`. |
| `TwistedSpells.h/.cpp` | `SpellScript`/`AuraScript` implementations for spell-driven features. |
| `conf/mod_twisted.conf.dist` | Template config with all supported keys and their defaults. |
| `data/sql/db-characters/` | SQL migrations for the `twisted_playerdata` character table. |

### Key Singleton Macro
```cpp
#define sTwistedMgr TwistedMgr::Get()
```
Use `sTwistedMgr->` everywhere outside `TwistedMgr` itself.

### Config Macro
```cpp
#define GET_CONF_VALUE(data_type, member_var, conf_str, default_val) \
    sTwistedMgr->member_var = sConfigMgr->GetOption<data_type>(conf_str, default_val);
```
Defined at the top of `TwistedConfig.cpp`. Use it for all scalar config reads.

---

## Code Style

- Use `const` for function parameters, return types, and local variables where appropriate.
- Use `constexpr` for compile-time constants.
- Use `auto` for type deduction when the type is obvious from the context.
- Use `nullptr` instead of `NULL`.

### Const Correctness for Local Variables

Local variables that are initialized once and never modified must be declared `const`:

```cpp
// Correct
const uint32 averageItemLevel = totalItemLevel / equippedCount;
const uint32 targetItemLevel = averageItemLevel + urand(1, 5);
const uint32 randomIndex = urand(0, candidateEntries.size() - 1);
const uint32 selectedEntry = candidateEntries[randomIndex];

// Wrong — do not use
uint32 averageItemLevel = totalItemLevel / equippedCount;
uint32 targetItemLevel = averageItemLevel + urand(1, 5);
uint32 randomIndex = urand(0, candidateEntries.size() - 1);
uint32 selectedEntry = candidateEntries[randomIndex];
```

Only omit `const` when the variable is intentionally modified after initialization (e.g., counters, accumulators).

## Conventions

### Config Key Naming
All config keys follow the pattern:
```
Twisted.FEATURENAME.VARIABLENAME
```
- `FEATURENAME` is PascalCase (e.g. `Rewards`, `ItemImbue`, `Treasures`).
- `VARIABLENAME` matches the field name in `TwistedMgr` exactly (e.g. `RewardGrantThreshold`).
- Every feature must have an enable toggle: `Twisted.FEATURENAME.Enable`.
- Every feature should have a verbose logging toggle: `Twisted.FEATURENAME.LogVerbose` (type `bool`, default `false`).

### Logging Rules
- Use `LOG_INFO`, `LOG_WARN`, or `LOG_ERROR` only. No `printf`, no `std::cout`.
- Always use `"twisted"` as the first argument (the log filter).
- Format arguments with `{}` placeholders — never `%s` / `%d` / `%u`.

```cpp
// Correct
LOG_INFO("twisted", "Player {} gained {} RewardXP.", player->GetName(), gainedXP);
LOG_WARN("twisted", "No RewardDefinition found for player {} at level {}.", player->GetName(), player->GetLevel());
LOG_ERROR("twisted", "Config key [{}] missing for Def {}!", sb.str(), Def);

// Wrong — do not use
LOG_INFO("twisted", "Player %s gained %u XP", player->GetName().c_str(), gainedXP);
```

- Verbose `LOG_INFO` calls (e.g., per-kill XP logging) must be guarded by the feature's `LogVerbose` flag:
```cpp
if (sTwistedMgr->RewardsLogVerbose)
    LOG_INFO("twisted", "...");
```

### `TwistedMgr` Field Grouping
Fields in `TwistedMgr.h` are grouped by feature under a comment block. Maintain this layout:
```cpp
// Treasure Find
bool TreasureFindEnabled = true;

// Item Imbues
bool ItemImbueEnabled = true;
// ...

// Rewards
bool RewardsEnabled = true;
// ...
```

### `friend class TwistedConfig`
`TwistedMgr` declares `friend class TwistedConfig`. This allows `TwistedConfig` to write directly into `private` fields via `GET_CONF_VALUE`. Do **not** add public setters for config fields — keep them private and loaded only through `TwistedConfig`.

---

## Workflow: Adding a New Feature / Config Block

**Goal:** Add a new feature `MyFeature` with an enable toggle, a verbose log toggle, and one or more scalar config values.

### 1. `TwistedMgr.h` — add private fields
```cpp
// My Feature
bool MyFeatureEnabled    = false;
bool MyFeatureLogVerbose = false;
uint32 MyFeatureValue    = 0;
```

### 2. `TwistedConfig.h` — declare a load function
```cpp
void LoadMyFeature();
```

### 3. `TwistedConfig.cpp` — implement and wire in

In `LoadConfig()`, add a call:
```cpp
LoadMyFeature();
```

Implement the function:
```cpp
void TwistedConfig::LoadMyFeature()
{
    GET_CONF_VALUE(bool,   MyFeatureEnabled,    "Twisted.MyFeature.Enable",     false);
    GET_CONF_VALUE(bool,   MyFeatureLogVerbose, "Twisted.MyFeature.LogVerbose", false);
    GET_CONF_VALUE(uint32, MyFeatureValue,      "Twisted.MyFeature.MyFeatureValue", 0);
}
```

### 4. `conf/mod_twisted.conf.dist` — document and set defaults
```ini
########################################
# My Feature
########################################
#
#    Twisted.MyFeature.Enable
#        Description: Enable My Feature
#        Default:     0 - Disabled
#                     1 - Enabled
#
#    Twisted.MyFeature.LogVerbose
#        Description: Log verbose debug info for My Feature
#        Default:     0 - Disabled
#
#    Twisted.MyFeature.MyFeatureValue
#        Description: Controls X for My Feature.
#        Default:     0
#

Twisted.MyFeature.Enable = 0
Twisted.MyFeature.LogVerbose = 0
Twisted.MyFeature.MyFeatureValue = 0
```

### 5. Guard logic in `TwistedMgr.cpp`
```cpp
void TwistedMgr::DoMyFeatureThing(Player* player)
{
    if (!MyFeatureEnabled)
        return;

    // ... logic ...

    if (MyFeatureLogVerbose)
        LOG_INFO("module", "Mod-Twisted: MyFeature did something for player {}.", player->GetName());
}
```

---

## Workflow: Adding a New Dynamic List (Numbered Structs)

Used when a feature needs N config-driven data records (like `RewardDefinitions` or `EnchantmentTiers`).

### 1. Define the struct in `TwistedMgr.h`
```cpp
struct MyItemData
{
    uint32 SomeId   = 0;
    uint32 MinLevel = 1;
    uint32 MaxLevel = 80;
};
```

### 2. Add fields to `TwistedMgr.h`
```cpp
// My Feature
uint32 NumMyItems = 0;
std::vector<MyItemData> MyItems;
```

### 3. Clear the vector in `TwistedConfig::OnShutdown()`
```cpp
sTwistedMgr->MyItems.clear();
```

### 4. Read count in the feature's `Load` function
```cpp
GET_CONF_VALUE(uint32, NumMyItems, "Twisted.MyFeature.NumItems", 0);
LoadMyItemDefinitions();
```

### 5. Implement `LoadMyItemDefinitions()` in `TwistedConfig.cpp`
```cpp
void TwistedConfig::LoadMyItemDefinitions()
{
    std::ostringstream sb;
    const std::string BaseConfStr = "Twisted.MyFeature.Item";
    sTwistedMgr->MyItems.clear();
    for (uint32 i = 0; i < sTwistedMgr->NumMyItems; ++i)
    {
        const uint32 N = i + 1;
        MyItemData Data;

        CLR_STREAM(sb)
        sb << BaseConfStr << N << ".SomeId";
        Data.SomeId = sConfigMgr->GetOption<uint32>(sb.str(), 0);
        if (Data.SomeId == 0)
            LOG_ERROR("module", "Mod-Twisted: Config key [{}] is missing or 0 for Item {}!", sb.str(), N);

        CLR_STREAM(sb)
        sb << BaseConfStr << N << ".MinLevel";
        Data.MinLevel = sConfigMgr->GetOption<uint32>(sb.str(), 1);

        CLR_STREAM(sb)
        sb << BaseConfStr << N << ".MaxLevel";
        Data.MaxLevel = sConfigMgr->GetOption<uint32>(sb.str(), 80);

        sTwistedMgr->MyItems.push_back(Data);
    }
}
```

### 6. Config entries follow the `Def1..DefN` / `Tier1..TierN` pattern
```ini
Twisted.MyFeature.NumItems = 2
Twisted.MyFeature.Item1.SomeId   = 12345
Twisted.MyFeature.Item1.MinLevel = 1
Twisted.MyFeature.Item1.MaxLevel = 29
Twisted.MyFeature.Item2.SomeId   = 12346
Twisted.MyFeature.Item2.MinLevel = 30
Twisted.MyFeature.Item2.MaxLevel = 59
```

---

## Workflow: Adding a New PlayerScript Hook

Used when `TwistedMgr` needs to respond to a player game event.

### 1. Find the hook enum value in `PlayerScript.h`
```
src/server/game/Scripting/ScriptDefines/PlayerScript.h
```
Look in the `PlayerHook` enum for the relevant `PLAYERHOOK_ON_*` constant and the matching virtual method signature.

### 2. `TwistedMgr.h` — declare the handler
```cpp
void OnPlayerSomeEvent(Player* player, /* other args */);
```

### 3. `TwistedMgr.cpp` — implement
```cpp
void TwistedMgr::OnPlayerSomeEvent(Player* player, /* args */)
{
    if (!MyFeatureEnabled)
        return;

    TwistedPlayerData* pData = GetOrFindPlayerData(player->GetGUID());
    if (!pData)
        return;

    // ... logic ...
}
```

### 4. `TwistedPlayer.h` — declare the override
```cpp
void OnPlayerSomeEvent(Player* player, /* args */) override;
```

### 5. `TwistedPlayer.cpp` — register hook + implement
In the constructor initializer list, add the hook constant:
```cpp
TwistedPlayer::TwistedPlayer() : PlayerScript("TwistedPlayer",
    {
        // ... existing hooks ...
        PLAYERHOOK_ON_SOME_EVENT,
    })
```

Add the forwarding implementation (one-liner to `sTwistedMgr`):
```cpp
void TwistedPlayer::OnPlayerSomeEvent(Player* player, /* args */)
{
    sTwistedMgr->OnPlayerSomeEvent(player, /* args */);
}
```

> **Note:** Both `OnPlayerCreatureKill` and `OnPlayerCreatureKilledByPet` forward into the same `TwistedMgr::OnPlayerCreatureKill` function — pet kills are treated identically to direct kills.

---

## Workflow: Adding a New Per-Player DB-Persisted Field

Used when a player value must survive server restarts.

### 1. `data/sql/db-characters/twisted_playerdata.sql` — add the column

The table is `twisted_playerdata (guid, rewardxp, ...)`. Add a new column:
```sql
ALTER TABLE `twisted_playerdata` ADD COLUMN `myfieldname` int unsigned NOT NULL DEFAULT '0';
```
Or update the `CREATE TABLE` statement if recreating from scratch.

### 2. `TwistedMgr.h` — add to `TwistedPlayerData`
```cpp
struct TwistedPlayerData
{
public:
    TwistedPlayerData()
        : TreasureFindPoints(0)
        , RewardXP(0)
        , MyFieldName(0)
    {
    }

    int32  TreasureFindPoints = 0;
    uint32 RewardXP           = 0;
    uint32 MyFieldName        = 0;
};
```

### 3. `TwistedMgr.cpp` — load in `PlayerInitialize`
Extend the `SELECT` query to include the new column and read the field:
```cpp
QueryResult result = CharacterDatabase.Query(
    "SELECT guid, rewardxp, myfieldname FROM twisted_playerdata WHERE guid = {}",
    player->GetGUID().GetCounter());

if (result)
{
    Field* fields = result->Fetch();
    pData->RewardXP    = fields[1].Get<uint32>();
    pData->MyFieldName = fields[2].Get<uint32>();
}
```

### 4. `TwistedMgr.cpp` — persist in `PlayerCleanup`
Extend the `REPLACE INTO` to include the new column:
```cpp
CharacterDatabase.Execute(
    "REPLACE INTO twisted_playerdata (guid, rewardxp, myfieldname) VALUES ({}, {}, {})",
    guid.GetCounter(),
    entry->second.RewardXP,
    entry->second.MyFieldName);
```

> `PlayerCleanup` is called from `TwistedPlayer::OnPlayerLogout`. It always runs the `REPLACE INTO` before erasing the in-memory entry.

---

## Quick Reference: `TwistedConfig` Load Chain

```
OnBeforeConfigLoad()
  └── LoadConfig()
        ├── GET_CONF_VALUE(TreasureFindEnabled, ...)
        ├── LoadImbueEnchantments()
        │     ├── GET_CONF_VALUE(ItemImbueEnabled, ...)
        │     └── LoadImbueEnchantmentTiers()   ← dynamic list
        └── LoadRewards()
              ├── GET_CONF_VALUE(RewardsEnabled, ...)
              ├── GET_CONF_VALUE(RewardGrantThreshold, ...)
              └── LoadRewardDefinitions()        ← dynamic list
```

New features slot in at the `LoadConfig()` level as a new `LoadFeatureName()` call.

---

## Quick Reference: Player Lifecycle

| Event | Hook | TwistedMgr function |
|---|---|---|
| Login | `PLAYERHOOK_ON_LOGIN` | `PlayerInitialize` — reads DB, populates `TwistedPlayerData` |
| Logout | `PLAYERHOOK_ON_LOGOUT` | `PlayerCleanup` — writes DB, erases in-memory entry |
| Creature kill (direct) | `PLAYERHOOK_ON_CREATURE_KILL` | `OnPlayerCreatureKill` |
| Creature kill (pet) | `PLAYERHOOK_ON_CREATURE_KILLED_BY_PET` | `OnPlayerCreatureKill` (same fn) |
| Quest turn-in | `PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST` | `OnPlayerCompleteQuest` |
| Level up | `PLAYERHOOK_ON_LEVEL_CHANGED` | `OnPlayerLevelChanged` |
