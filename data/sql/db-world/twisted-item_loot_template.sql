DELETE FROM `item_loot_template` WHERE `entry` = 60046;

INSERT INTO `item_loot_template` VALUES (60046,0,2222000,100,0,1,0,1,1,'Treasure Box lvl 1 - Guaranteed Twist');
INSERT INTO `item_loot_template` VALUES (60046,858,0,15,0,1,0,1,1,'Treasure Box lvl 1 - LesserHealPot');
INSERT INTO `item_loot_template` VALUES (60046,1127,0,3,0,1,0,1,2,'Treasure Box lvl 1 - Flash Bundle');
INSERT INTO `item_loot_template` VALUES (60046,1178,0,10,0,1,0,1,5,'Treasure Box lvl 1 - Explosive Rocket');
INSERT INTO `item_loot_template` VALUES (60046,3829,0,1.5,0,1,0,1,1,'Treasure Box lvl 1 - Frost Oil');
INSERT INTO `item_loot_template` VALUES (60046,4498,0,5,0,1,0,1,1,'Treasure Box lvl 1 - 6 slot bag');
INSERT INTO `item_loot_template` VALUES (60046,5013,0,8,0,1,0,1,1,'Treasure Box lvl 1 - FertileBulb');
INSERT INTO `item_loot_template` VALUES (60046,38773,0,8,0,1,0,1,1,'Treasure Box lvl 1 - EnchChestLesserHP');
INSERT INTO `item_loot_template` VALUES (60046,38776,0,8,0,1,0,1,1,'Treasure Box lvl 1 - EnchChestLesserMP');

DELETE FROM `reference_loot_template` WHERE `entry` BETWEEN 2222000 AND 2223000;

INSERT INTO `reference_loot_template` VALUES (2222000,60044,0,50,0,1,0,1,1,'lvl 1 - Weapon Twist');
INSERT INTO `reference_loot_template` VALUES (2222000,60045,0,50,0,1,0,1,1,'lvl 1 - Armor Twist');