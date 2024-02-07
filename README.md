# Hardcore Mod
Mode to make our life harder while playing wow (because why not?). Only for SPP vanilla at the moment.

This mod allows you to configure the following:
- If a grave with a custom message should spawn on player death
- If the player should drop gear, bag items and/or gold when dying (also for bots)
- If the above loot should spawn when dying in a fancy chest
- The amount of loot allowed to spawn after multiple deaths (the oldest loot will disappear)
- If the player should lose levels/xp when dying
- If the player is allowed to revive after death
- If the player should auto revive in the nearest graveyard instead of running back to the corpse as a ghost

# How to install
NOTE: This guide assumes that you have basic knowledge on how to generate c++ projects, compile code and modify a database

1. You should use a compatible core version for this to work: 
- Classic (Hardcore only): https://github.com/davidonete/mangos-classic/tree/hardcore-mod
- Classic (All mods): https://github.com/celguar/mangos-classic/tree/ike3-bots
- TBC (Hardcore only): To be done...
- TBC (All mods): https://github.com/celguar/mangos-tbc/tree/ike3-bots
- WoTLK (Hardcore only): To be done...
- WoTLK (All mods): https://github.com/celguar/mangos-wotlk/tree/ike3-bots

NOTE: The "Hardcore only" version is provided as an example of where to place the code on the cmangos core in order to make it work, however it won't get updated to the latest core version unless the affected changes require it.

2. Clone the core desired and generate the solution using cmake. This mod requires you to enable the "BUILD_HARDCORE" flag for it to compile.

3. Build the project.

4. Copy the configuration file from "src/hardcore.conf.dist.in" and place it where your mangosd executable is. Also rename it to "hardcore.conf".

5. Remember to edit the config file and modify the options you want to use.

6. Lastly you will have to install the database changes located in the "sql/install" folder, each folder inside represents where you should execute the queries. E.g. The queries inside of "sql/install/world" will need to be executed in the world/mangosd database, the ones in "sql/install/characters" in the characters database, etc...

# How to uninstall
To remove achievements from your server you have multiple options, the first and easiest is to disable it from the hardcore.conf file. The second option is to completely remove it from the server and db:

1. Remove the "BUILD_HARDCORE" flag from your cmake configuration and recompile the game

2. Execute the sql queries located in the "sql/uninstall" folder. Each folder inside represents where you should execute the queries. E.g. The queries inside of "sql/uninstall/world" will need to be executed in the world/mangosd database, the ones in "sql/uninstall/characters" in the characters database, etc...
