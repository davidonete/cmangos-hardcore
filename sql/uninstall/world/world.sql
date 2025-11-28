DELETE FROM gameobject_template WHERE `type`=2 AND `CustomData1`=3643;

SET @Entry = 190011;
DELETE FROM `creature_template` WHERE `entry` = @Entry;
DELETE FROM `locales_creature` WHERE entry = @Entry;
DELETE FROM `creature` WHERE `id` = @Entry;

SET @TEXT_ID := 50900;
DELETE FROM `npc_text` WHERE `ID` BETWEEN @TEXT_ID AND @TEXT_ID+16;
DELETE FROM `locales_npc_text` WHERE `entry` BETWEEN @TEXT_ID AND @TEXT_ID+16;

SET @STRING_ENTRY := 12200;
DELETE FROM `mangos_string` WHERE `entry` BETWEEN @STRING_ENTRY AND @STRING_ENTRY+14;

SET @START_SPELL_ID := 33500;
SET @END_SPELL_ID := @START_SPELL_ID+1;
DELETE FROM `spell_template` WHERE `Id` BETWEEN  @START_SPELL_ID AND @END_SPELL_ID;