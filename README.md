# Hardcore Mod
Mode to allow changing your equipped item appearances with ones in your bags or banks. This tries to simulate the transmog feature introduced in Cataclysm, with the exception that only items in your posession can be used (not every item you ever collected).

# How to install
NOTE: This guide assumes that you have basic knowledge on how to generate c++ projects, compile code and modify a database

1. You should use a compatible core version for this to work: 
- Classic (Transmog only): https://github.com/davidonete/mangos-classic/tree/transmog-mod
- Classic (All mods): https://github.com/celguar/mangos-classic/tree/ike3-bots
- TBC (Transmog only): To be done...
- TBC (All mods): https://github.com/celguar/mangos-tbc/tree/ike3-bots
- WoTLK (Transmog only): To be done...
- WoTLK (All mods): https://github.com/celguar/mangos-wotlk/tree/ike3-bots

NOTE: The "Transmog only" version is provided as an example of where to place the code on the cmangos core in order to make it work, however it won't get updated to the latest core version unless the affected changes require it.

2. Clone the core desired and generate the solution using cmake. This mod requires you to enable the "BUILD_TRANSMOG" flag for it to compile.

3. Build the project.

4. Copy the configuration file from "src/transmog.conf.dist.in" and place it where your mangosd executable is. Also rename it to "transmog.conf".

5. Remember to edit the config file and modify the options you want to use.

6. Lastly you will have to install the database changes located in the "sql/install" folder, each folder inside represents where you should execute the queries. E.g. The queries inside of "sql/install/world" will need to be executed in the world/mangosd database, the ones in "sql/install/characters" in the characters database, etc...

# How to uninstall
To remove transmog from your server you have multiple options, the first and easiest is to disable it from the hardcore.conf file. The second option is to completely remove it from the server and db:

1. Remove the "BUILD_TRANSMOG" flag from your cmake configuration and recompile the game

2. Execute the sql queries located in the "sql/uninstall" folder. Each folder inside represents where you should execute the queries. E.g. The queries inside of "sql/uninstall/world" will need to be executed in the world/mangosd database, the ones in "sql/uninstall/characters" in the characters database, etc...
