# Hardcore
Module to make our life harder while playing wow (because why not?)

This module allows you to configure the following:
- If a grave with a custom message should spawn on player death
- If the player should drop gear, bag items and/or gold when dying (also for bots if playerbots are enabled)
- If the above loot should spawn when dying in a fancy chest
- The amount of loot allowed to spawn after multiple deaths (the oldest loot will disappear)
- If the player should lose levels/xp when dying
- If the player is allowed to revive after death
- If the player should auto revive in the nearest graveyard instead of running back to the corpse as a ghost

# Available Cores
Classic, TBC and WoTLK

# How to install
1. Follow the instructions in https://github.com/davidonete/cmangos-modules?tab=readme-ov-file#how-to-install
2. Enable the `BUILD_MODULE_HARDCORE` flag in cmake and run cmake. The module should be installed in `src/modules/hardcore`
4. Copy the configuration file from `src/modules/hardcore/src/hardcore.conf.dist.in` and place it where your mangosd executable is. Also rename it to `hardcore.conf`.
5. Remember to edit the config file and modify the options you want to use.
6. Lastly you will have to install the database changes located in the `src/modules/hardcore/sql/install` folder, each folder inside represents where you should execute the queries. E.g. The queries inside of `src/modules/hardcore/sql/install/world` will need to be executed in the world/mangosd database, the ones in `src/modules/hardcore/sql/install/characters` in the characters database, etc...

# How to uninstall
To remove the hardcore module from your server you have multiple options, the first and easiest is to disable it from the `hardcore.conf` file. The second option is to completely remove it from the server and db:
1. Remove the `BUILD_MODULE_HARDCORE` flag from your cmake configuration and recompile the game
2. Execute the sql queries located in the `src/modules/hardcore/sql/uninstall` folder. Each folder inside represents where you should execute the queries. E.g. The queries inside of `src/modules/hardcore/sql/uninstall/world` will need to be executed in the world/mangosd database, the ones in `src/modules/hardcore/sql/uninstall/characters` in the characters database, etc...
