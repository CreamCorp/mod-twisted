DROP TABLE IF EXISTS `twisted_playerdata`;

CREATE TABLE IF NOT EXISTS `twisted_playerdata` (
  `guid` int unsigned NOT NULL DEFAULT '0',
  `rewardxp` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=UTF8MB4 ROW_FORMAT=DYNAMIC;