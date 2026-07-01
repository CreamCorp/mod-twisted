DELETE FROM `item_loot_template` WHERE `entry` = 60046;

INSERT INTO `item_loot_template` VALUES (60046,0,2222000,100,0,1,0,1,1,'Treasure Box lvl 1 - Guaranteed Twist');
INSERT INTO `item_loot_template` VALUES (60046,0,2222001,100,0,1,0,1,2,'Treasure Box lvl 1 - Guaranteed Consumable');

DELETE FROM `reference_loot_template` WHERE `entry` BETWEEN 2222000 AND 2223000;

INSERT INTO `reference_loot_template` VALUES (2222000,60044,0,50,0,1,0,1,1,'lvl 1 - Weapon Twist');
INSERT INTO `reference_loot_template` VALUES (2222000,60045,0,50,0,1,0,1,1,'lvl 1 - Armor Twist');
INSERT INTO `reference_loot_template` VALUES (2222001,60047,0,50,0,1,0,1,1,'lvl 1 - Luck Potion');
INSERT INTO `reference_loot_template` VALUES (2222001,858,0,20,0,1,0,1,1,'lvl 1 - LesserHealPot');
INSERT INTO `reference_loot_template` VALUES (2222001,4498,0,10,0,1,0,1,1,'lvl 1 - 6 slot bag');
INSERT INTO `reference_loot_template` VALUES (2222001,1127,0,8,0,1,0,1,2,'lvl 1 - Flash Bundle');
INSERT INTO `reference_loot_template` VALUES (2222001,1178,0,8,0,1,0,1,5,'lvl 1 - Explosive Rocket');
INSERT INTO `reference_loot_template` VALUES (2222001,5013,0,8,0,1,0,1,1,'lvl 1 - FertileBulb');
INSERT INTO `reference_loot_template` VALUES (2222001,60047,0,3,0,1,0,1,1,'lvl 1 - Greater Luck Potion');
INSERT INTO `reference_loot_template` VALUES (2222001,3829,0,3,0,1,0,1,1,'lvl 1 - Frost Oil');
INSERT INTO `reference_loot_template` VALUES (2222001,38773,0,5,0,1,0,1,1,'lvl 1 - EnchChestLesserHP');
INSERT INTO `reference_loot_template` VALUES (2222001,38776,0,5,0,1,0,1,1,'lvl 1 - EnchChestLesserMP');