SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


-- /////////////////////////////////////////////////////////////////////////////////////////////////
-- //
-- //  База данных: `tegia_core`
-- //
-- /////////////////////////////////////////////////////////////////////////////////////////////////


CREATE DATABASE IF NOT EXISTS `tegia_core` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
USE `tegia_core`;


-- /////////////////////////////////////////////////////////////////////////////////////////////////
-- //
-- //  Структура таблицы `_jobs`
-- //
-- /////////////////////////////////////////////////////////////////////////////////////////////////





-- /////////////////////////////////////////////////////////////////////////////////////////////////
-- //
-- //  Структура таблицы `subscribers`
-- //
-- /////////////////////////////////////////////////////////////////////////////////////////////////


CREATE TABLE IF NOT EXISTS `subscribers` (
	`_id` bigint(20) UNSIGNED ZEROFILL NOT NULL AUTO_INCREMENT,
	`emiter_type` varchar(250) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
	`emiter_id` varchar(250) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
	`event_type` varchar(250) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
	`actor` varchar(250) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
	`action` text CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
	PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- /////////////////////////////////////////////////////////////////////////////////////////////////
-- //
-- //  Структура таблицы `events_log`
-- // 
-- /////////////////////////////////////////////////////////////////////////////////////////////////


CREATE TABLE IF NOT EXISTS `events_log` (
	`_id` bigint(20) UNSIGNED ZEROFILL NOT NULL,
	`emiter_type` varchar(250) NOT NULL,
	`emiter_id` varchar(250) NOT NULL,
	`event_type` varchar(250) NOT NULL,
	`t_create` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`data` json NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


--
-- Indexes for table `event_log`
--
ALTER TABLE `events_log`
	ADD PRIMARY KEY (`_id`),
	ADD KEY `emiter_type` (`emiter_type`),
	ADD KEY `emiter_id` (`emiter_id`),
	ADD KEY `event_type` (`event_type`),
	ADD KEY `t_create` (`t_create`);

--
-- AUTO_INCREMENT for table `event_log`
--
ALTER TABLE `events_log`
	MODIFY `_id` bigint(20) UNSIGNED ZEROFILL NOT NULL AUTO_INCREMENT;


-- /////////////////////////////////////////////////////////////////////////////////////////////////
-- //
-- //  Структура таблицы `workspaces`
-- //
-- /////////////////////////////////////////////////////////////////////////////////////////////////


CREATE TABLE IF NOT EXISTS `workspaces` (
	`_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT ,
	`type` INT UNSIGNED NOT NULL ,
	`name` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
	`data` JSON NOT NULL ,
	PRIMARY KEY (`_id`),
	INDEX (`type`))
ENGINE = InnoDB CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE `workspaces` ADD `owner` BIGINT UNSIGNED NOT NULL AFTER `name`, ADD INDEX (`owner`);


-- /////////////////////////////////////////////////////////////////////////////////////////////////
-- //
-- //  Структура таблицы `names`
-- //
-- /////////////////////////////////////////////////////////////////////////////////////////////////



