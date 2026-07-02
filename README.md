# Twisted Azeroth

This mod includes a number of custom functionality created for the Twisted fork of my [NpcBots](https://github.com/CreamCorp/azerothcore-wotlk-npcbots) repository.

## Features
### Reward System

A reward system that grants experience points and items to players based on various in-game activities. Players earn XP through creature kills, quest turn-ins, and level-ups, with configurable thresholds and rewards.

Rewards are configured in groups of Definitions, where a player is elligible for the specified item id if they fall within the min and max level. Any number of definitions are possible by increasing NumDefs and duplicating the Def# entries, then changing the # to the next available index. Note that defs begin at index 1.

**Configuration Options:**
- `Twisted.Rewards.Enable` - Enable/disable the system
- `Twisted.Rewards.StartingRewardXP` - Initial XP reward amount
- `Twisted.Rewards.RewardGrantThreshold` - XP threshold for granting rewards
- `Twisted.Rewards.LevelUpXP` - XP granted on level up
- `Twisted.Rewards.OnCreatureKillInLevelRangeXP` - XP for killing creatures in level range
- `Twisted.Rewards.OnCreatureKillAnyLevelXP` - XP for killing any creature
- `Twisted.Rewards.OnQuestTurnInXp` - XP for quest completion
- `Twisted.Rewards.NumDefs` - Number of reward definitions
- `Twisted.Rewards.Def#.RewardItemId` - Item ID for reward definitions
- `Twisted.Rewards.Def#.MinLevel` - Minimum level for reward
- `Twisted.Rewards.Def#.MaxLevel` - Maximum level for reward

### Item Imbue System

An enchantment system that ties to the "Scroll of Armor Twisting" and "Scroll of Weapon Twisting" items. When these enchantments are used on a target item, we perform a lookup from the source item ids to determine what enchantments to apply. There is a chance for a 'weapon twist' to pull from the list of unique weapon enchantments, and the same goes for armor. These unique lists are intended to be extreme power boosts and therefor a low chance of triggering - they do not scale with the player level or tiers. There is also a chance for an enchantment to 'upgrade' into the next available tier.

Like the rewards above, enchantments are configured in groups of Tiers. Any number of Tiers are possible by increasing NumTiers and duplicating the Tier# entries, then changing the # to the next available index. Note that Tiers begin at index 1.

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

### Treasure Find System

Some items, buffs, and enchantments provide a "treasure find" bonus that increases the chance of finding special items during a LootProcess. Currently, the system will try to award the player with a moderate ilevel upgrade for the current average ilevel of equipped gear.