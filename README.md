# Twisted Azeroth

This mod includes a number of custom functionality created for the Twisted fork of my [NpcBots](https://github.com/CreamCorp/azerothcore-wotlk-npcbots) repository.

## Features

### Treasure Find System

A reward system that grants experience points and items to players based on various in-game activities. Players earn XP through creature kills, quest turn-ins, and level-ups, with configurable thresholds and rewards.

**Configuration Options:**
- `Twisted.Rewards.Enable` - Enable/disable the system
- `Twisted.Rewards.StartingRewardXP` - Initial XP reward amount
- `Twisted.Rewards.RewardGrantThreshold` - XP threshold for granting rewards
- `Twisted.Rewards.LevelUpXP` - XP granted on level up
- `Twisted.Rewards.OnCreatureKillInLevelRangeXP` - XP for killing creatures in level range
- `Twisted.Rewards.OnCreatureKillAnyLevelXP` - XP for killing any creature
- `Twisted.Rewards.OnQuestTurnInXp` - XP for quest completion
- `Twisted.Rewards.Def#.RewardItemId` - Item ID for reward definitions
- `Twisted.Rewards.Def#.MinLevel` - Minimum level for reward
- `Twisted.Rewards.Def#.MaxLevel` - Maximum level for reward

### Item Imbue System

An enchantment system that allows items to receive random enchantments when imbued. Supports tiered enchantments with upgrade chances, and unique enchantment pools for armor and weapons.

**Configuration Options:**
- `Twisted.ItemImbue.Enable` - Enable/disable the system
- `Twisted.ItemImbue.ArmorUniqueChance` - Chance for unique armor enchantments
- `Twisted.ItemImbue.ArmorUniqueEnchantIds` - Unique armor enchantment IDs
- `Twisted.ItemImbue.WeaponUniqueChance` - Chance for unique weapon enchantments
- `Twisted.ItemImbue.WeaponUniqueEnchantIds` - Unique weapon enchantment IDs
- `Twisted.ItemImbue.TierUpgradeChance` - Chance to upgrade to next tier
- `Twisted.ItemImbue.NumTiers` - Number of enchantment tiers
- `Twisted.ItemImbue.Tier#.ItemIds` - Source items for each tier
- `Twisted.ItemImbue.Tier#.EnchantIds` - Enchantment IDs for each tier