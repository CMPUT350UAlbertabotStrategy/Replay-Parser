#include "Options.h"

namespace Options
{
	namespace Modules							// toggle various modules of UAlbertaBot, must be const
	{
		bool const USING_GAMECOMMANDER				= false;		// toggle GameCommander, effectively UAlbertaBot
		bool const USING_ENHANCED_INTERFACE			= false;		// toggle EnhancedUI, not needed for UAlbertaBot
		bool const USING_REPLAY_VISUALIZER			= false;		// cannot be on while gamecommander is on
		bool const USING_MICRO_SEARCH				= false;		// toggle use of Micro Search, if false script used
		bool const USING_MACRO_SEARCH				= false;		// toggle use of Build Order Search, currently no backup
		bool const USING_STRATEGY_IO				= false;		// toggle the use of file io for strategy
	}

	namespace Tournament						// settings for the AIIDE tournament
	{
		extern const int GAME_END_FRAME			= 86400;	
	}

	namespace FileIO							// settings for file read/write
	{
		extern const char * FILE_SETTINGS		= "file_settings.txt";
	}

	namespace Debug								// debugging options
	{
		bool DRAW_UALBERTABOT_DEBUG				= true;		// draws debug information for UAlbertaBot
		bool DRAW_DEBUG_INTERFACE				= false;		// draws debug information for EnhancedUI

		BWAPI::Color COLOR_LINE_TARGET			= BWAPI::Colors::White;
		BWAPI::Color COLOR_LINE_MINERAL			= BWAPI::Colors::Cyan;
		BWAPI::Color COLOR_UNIT_NEAR_ENEMY		= BWAPI::Colors::Red;
		BWAPI::Color COLOR_UNIT_NOTNEAR_ENEMY	= BWAPI::Colors::Green;
	}

	namespace Micro								// micromanagement options
	{
		bool WORKER_DEFENSE						= true;		// whether or not we defend with workers when combat units die
		int WORKER_DEFENSE_PER_UNIT				= 2;		// how many workers to assign to each unit attacking us
		
		int COMBAT_RADIUS						= 1000;		// radius of combat to consider units for Micro Search
		int COMBAT_REGROUP_RADIUS				= 300;		// radius of units around frontmost unit we consider in regroup calculation
		int UNIT_NEAR_ENEMY_RADIUS				= 600;		// radius to consider a unit 'near' to an enemy unit
		int MICRO_SEARCH_MAX_TIMER				= 0;		// maximum amount of time to allot to micro search (ms)
		int MICRO_SEARCH_MIN_TIMER				= 0;		// minimum amount of time to allot to micro search (ms)
	}

	namespace Tools								// options for various tools
	{
		extern int MAP_GRID_SIZE				= 320;		// size of grid spacing in MapGrid
	}
}