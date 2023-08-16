SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";

--
--
--

USE `tegia_core`;

/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;


-- ------------------------------------------------------------------------------------------
--
-- Table structure for table `_jobs`
--
-- ------------------------------------------------------------------------------------------


CREATE TABLE IF NOT EXISTS `_jobs` (
	`_id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
	`_id_case` bigint(20) UNSIGNED NOT NULL,
	`config` varchar(250) NOT NULL COMMENT 'Config name',
	`time` varchar(250) NOT NULL,
	`actor` varchar(250) NOT NULL,
	`action` text NOT NULL,
	`message` text NOT NULL,
	`status` int(10) UNSIGNED NOT NULL,
	PRIMARY KEY (`_id`),
	KEY `config` (`config`),
	KEY `_id_case` (`_id_case`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;


-- ------------------------------------------------------------------------------------------
--
-- Data for table `_jobs`
--
-- ------------------------------------------------------------------------------------------


INSERT IGNORE INTO `_jobs` (`_id`, `_id_case`, `config`, `time`, `actor`, `action`, `message`, `status`) VALUES
(1, 0, 'base', '0 0 0 * * *', 'scheduler', '/test1', '{ \"test\": \"test\" }', 1),
(2, 1, 'government', '0 15 19 * * *', 'government/passports/invalid', '/pipeline', '{  }', 1);



COMMIT;
