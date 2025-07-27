DELETE FROM gameobject_template WHERE `type`=2 AND `CustomData1`=3643;

SET @Entry := 190011;

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `DisplayId1`, `DisplayIdProbability1`, `name`, `subname`, `GossipMenuId`, `minlevel`, `maxlevel`, `faction`, `NpcFlags`, `scale`, `rank`, `DamageSchool`, `MeleeBaseAttackTime`, `RangedBaseAttackTime`, `unitClass`, `unitFlags`, `CreatureType`, `CreatureTypeFlags`, `lootid`, `PickpocketLootId`, `SkinningLootId`, `AIName`, `MovementType`, `RacialLeader`, `RegenerateStats`, `MechanicImmuneMask`, `ExtraFlags`) VALUES
(@Entry, 1403, 100, "\"Masochist\" Pete", "Challenge Advisor", 0, 60, 60, 35, 1, 1, 0, 0, 2000, 0, 1, 0, 7, 138936390, 0, 0, 0, '', 0, 0, 1, 0, 0);

DELETE FROM `locales_creature` WHERE `entry` = @Entry;
INSERT INTO `locales_creature` (`entry`, `name_loc6`, `subname_loc6`) VALUES (@Entry, "Pedro \"El Masoca\"", 'Asesor de Desafios');

DELETE FROM `creature` WHERE `id` = @Entry;
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 0, 1, -8999.00000000000000000000, 851.19100000000000000000, 29.62100000000000000000, 3.88538000000000000000, 25, 25, 0, 0);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 1, 1, 1467.40000000000000000000, -4226.33000000000000000000, 58.99390000000000000000, 1.19063000000000000000, 25, 25, 0, 0);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 0, 1, -8903.58000000000000000000, -108.40100000000000000000, 81.84860000000000000000, 4.08677000000000000000, 25, 25, 0, 0);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 0, 1, -6213.26000000000000000000, 330.66400000000000000000, 383.71900000000000000000, 2.89842000000000000000, 25, 25, 0, 0);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 1, 1, 10327.10000000000000000000, 822.52100000000000000000, 1326.43000000000000000000, 2.53681000000000000000, 25, 25, 0, 0);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 1, 1, -638.98100000000000000000, -4227.08000000000000000000, 38.13420000000000000000, 5.47014000000000000000, 25, 25, 0, 0);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 0, 1, 1859.94000000000000000000, 1560.67000000000000000000, 99.07910000000000000000, 1.57723000000000000000, 25, 25, 0, 0);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES (@Entry, 1, 1, -2882.11000000000000000000, -277.04500000000000000000, 53.91540000000000000000, 2.37644000000000000000, 25, 25, 0, 0);

SET @TEXT_ID := 50900;
DELETE FROM `npc_text` WHERE `ID` BETWEEN @TEXT_ID AND @TEXT_ID+9;
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(@TEXT_ID,   "Ahoy, $N. If you are looking for a challenge you have come to the right place. Tell me, what challenge are you interested in?"),
(@TEXT_ID+1, "Appologines $N. I can't provide you with any challenges at the moment."),
(@TEXT_ID+2, "Oh, the hardcore challenge you say? I thought you'd never ask... If you accept this challenge, it means you will only have one life and if you die, that's it, no more retries. Are you up for it?"),
(@TEXT_ID+3, "Hmm, the drop loot challenge? This challenge will make you drop some of your belongings every time you die, including gear and gold. What do you say?"),
(@TEXT_ID+4, "Ah, the classic lose experience challenge. This challenge will make you lose some experience every time you die, and if you die enough you can even lose levels. Interested?"),
(@TEXT_ID+5, "Hold your horses cowboy! It seems like your journey has already taken its course. To start this challenge you must speak with me with a fresh start, if you know what I mean..."),
(@TEXT_ID+6, "I know you are excited about this challenge, but you seem like you already accepted it. What do you want, to double accept it?"),
(@TEXT_ID+7, "Hahaha! That's what I'm talking about! Good luck! You will need it..."),
(@TEXT_ID+8, "Oh, so you don’t want people attacking you during the challenge? That’s cute... But hey, who am I to judge..."),
(@TEXT_ID+9, "All right then... Consider it done. Careful not to break a nail out there.");

DELETE FROM `locales_npc_text` WHERE `entry` BETWEEN @TEXT_ID AND @TEXT_ID+9;
INSERT INTO `locales_npc_text` (`entry`, `text0_0_loc6`) VALUES
(@TEXT_ID,   "¡Hola, $N! Si estas buscando un reto, estás en el lugar indicado. Dime, ¿qué reto te interesa?"),
(@TEXT_ID+1, "Mis disculpas $N. No tengo ningun desafío disponible en estos momentos."),
(@TEXT_ID+2, "¿El desafío hardcore dices? Creí que nunca me lo preguntarías... Si aceptas este desafío, significa que solo tendrás una vida y, si mueres, se acabó, no habrá más reintentos. ¿Te animas?"),
(@TEXT_ID+3, "Mmm, ¿el desafío de perder posesiones? Este desafío te hará soltar algunas de tus pertenencias cada vez que mueras, incluyendo equipo y oro. ¿Qué me dices?"),
(@TEXT_ID+4, "Ah, el clásico desafío de perder experiencia. Este desafío te hará perder experiencia cada vez que mueras, y si mueres lo suficiente, incluso puedes perder niveles. ¿Te interesa?"),
(@TEXT_ID+5, "¡Calma, vaquero! Parece que tu viaje ya ha tomado su curso. Para empezar este reto, debes hablar conmigo desde el principio, si sabes a que me refiero..."),
(@TEXT_ID+6, "Sé que estás entusiasmado con este reto, pero parece que ya lo has aceptado. ¿Qué quieres, aceptarlo dos veces?"),
(@TEXT_ID+7, "¡Jajaja! ¡Eso es lo que queria escuchar! ¡Mucha suerte! La necesitarás..."),
(@TEXT_ID+8, "¿Ah, entonces no quieres que otras personas te ataquen durante el desafío? Qué monada... Pero bueno, ¿quién soy yo para juzgar?"),
(@TEXT_ID+9, "De acuerdo, ya estas listo. Ve con cuidado por ahi, no vaya a ser que te rompas una uña.");

SET @STRING_ENTRY := 12200;
DELETE FROM `mangos_string` WHERE `entry` BETWEEN @STRING_ENTRY AND @STRING_ENTRY+7;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc6`) VALUES 
(@STRING_ENTRY,   "I'm interested in the hardcore challenge", "Estoy interesado en el desafío hardcore"),
(@STRING_ENTRY+1, "I'm interested in the drop loot challenge", 'Estoy interesado en el desafío de perder posesiones'),
(@STRING_ENTRY+2, "I'm interested in the lose experience", 'Estoy interesado en el desafío de perder experiencia'),
(@STRING_ENTRY+3, "I accept the challenge!", '¡Acepto el desafío!'),
(@STRING_ENTRY+4, "Maybe later...", 'Quizas mas tarde...'),
(@STRING_ENTRY+5, "I would like to disable pvp fights", 'Me gustaria desactivar el combate pvp'),
(@STRING_ENTRY+6, "Yes, please!", '¡Si, por favor!'),
(@STRING_ENTRY+7, "Maybe not...", 'Quizas no...');