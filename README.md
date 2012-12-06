Replay-Parser
=============
Brendan Anthony
Anthony Ou


The wiki can be found at our github
https://github.com/CMPUT350UAlbertabotStrategy/Replay-Parser/wiki

A parsing system for retrieving strategic information from BroodWarAPI replays

Modifications are Mainly in the K Means Clustering Folder
and the UAlbertaBot folder, particularly in files ReplayModule.cpp
and ReplayModule.h, some if statements in UalbertaBotModule.cpp and a bug fix in 
informationmanager.h

==============================================================================


Every replay has its file name printed and the players who are protoss
If no one is protoss the replay is not used and 
after every game there will be a print statement of number of units made.

After the replays are all a vector of vectors is passed and clusters are generated.



===========================================
Each name corresponds to an index in each feature vector or cluster
Arbiter starting at index 0

	Arbiter,Archon,ArbiterTribunal,Assimilator,
	Carrier,Corsair,CitadelofAdun,CyberneticsCore,
	Darkarchon,DarkTemplar,Dragoon,
	HighTemplar,Interceptor,Observer,Probe,Reaver,
	Scout,Shuttle,Zealot,Gateway,Nexus,
	Observatory,FleetBeacon,Forge,Photoncannon,
	Pylon,RoboticsFacility,RoboticsSupportbay,
	Shieldbattery,Stargate,TemplarArchives

===========================================



Kmeans Parser uses linux ls and cat programs(untested and incomplete implimentation) the windows dir and an included coreutils for 
windows. Put the compiled K Cleans Clustering.exe in the same folder as replay files and install the coreutils.exe included to have windows access to cat, with windows path directed to the installed coreutils/bin folder.

This program uses most of the same dependences as UAlbertaBot except for coreutils
http://gnuwin32.sourceforge.net/packages/coreutils.htm

=============================================================================
========================New Features tracklist===============================

Replay parsing abilities are in a .cpp and .h file called "ReplayModule" they do not inherit from the AiModule from BWAPI, but get called from UalbertaModule on game events.

struct gamesummary

This struct contains two vectors, one which holds units made and units destroyed throughout the entire game. Units are never removed from these vectors until Ualbertabot.dll is unloaded from stracraft.

struct data

This struct contains an integer of the frame when the unit was destroyed or created. An integer representing the player -1, 0, 1 and so on, but most games involve 1 vs 1. Lastly there is a string which holds the unit name.

struct gamecountinterval

This stuct is returned when the game is parsed into some interval by the summarytointerval() function. It doesnt use vectors but a map of all units destroyed and created stored in a map within the defined intervals. This is sorted first by player ID and then by the unit name.

These are called only during a replay

void onStart()

Called to initalize datastructures and print of one time print statements.

void onFrame()

called every frame%12 and handles detection, not necessary for this project. the BWAPI function onunitshow isn't used because it is verbose. For example it will print that player 1 created a unit and in the next line it will print that player 1 detected the same unit.

void onEnd(bool isWinner) This is when any point of the game can be parsed.

on game vent function calls. void onUnitDestroy(BWAPI::Unit * unit) void onUnitMorph(BWAPI::Unit * unit) void onUnitRenegade(BWAPI::Unit * unit) void onUnitCreate(BWAPI::Unit * unit)

Below are vision related functions and gets called from onFrame events void checkVision(BWAPI::Unit * unit) void handleVisionEvents() void print(std::string event, BWAPI::Unit * unit)

This prints into a file, for debugging or latter parsing.

gamecountinterval * summarytointerval(int lowerboundry, int higherboundry)

parses the summary of the game, the two big vectors holding all units made and destroyed into a gamecountinterval struct object.
============================================================================================

Kmeans.cpp
Vectors have to be the same size and there are limited amounts of type checking but this is taken care of by the parser to some extent.




