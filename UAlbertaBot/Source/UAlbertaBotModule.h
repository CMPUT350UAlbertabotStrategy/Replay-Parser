#pragma once

#include <BWAPI.h>
#include "GameCommander.h"
#include <iostream>
#include <fstream>
#include "Logger.h"
#include "MapTools.h"
#include "HardCodedInfo.h"
#include "../../StarcraftBuildOrderSearch/Source/starcraftsearch/StarcraftData.hpp"
#include "MicroSearchManager.h"
#include "ReplayVisualizer.h"
#include <iomanip>

#include "EnhancedInterface.hpp"

#include "Options.h"

class UAlbertaBotModule : public BWAPI::AIModule
{
	GameCommander			gameCommander;
	EnhancedInterface		eui;
	MicroSearchManager		micro;

public:
			
	UAlbertaBotModule();
	~UAlbertaBotModule();

	void	onStart();
	void	onFrame();
	void	onEnd(bool isWinner);
	void	onUnitDestroy(BWAPI::Unit * unit);
	void	onUnitMorph(BWAPI::Unit * unit);
	void	onSendText(std::string text);
	void	onUnitCreate(BWAPI::Unit * unit);
	void	onUnitShow(BWAPI::Unit * unit);
	void	onUnitHide(BWAPI::Unit * unit);
	void	onUnitRenegade(BWAPI::Unit * unit);

private:
	//new functions and variables
	std::ofstream replayDat;
	std::map<BWAPI::Player*, std::set<BWAPI::Unit*> > seenThisTurn;
	std::set<BWAPI::Player*> activePlayers;
	std::map<BWAPI::Player*, std::set<std::pair<BWAPI::Unit*, BWAPI::UnitType>>> unseenUnits;

	//holds ALL unit(and id of player) and the time they were made.
	struct data{
		data(std::string unit, int frame, int player): unit(unit), frame(frame), player(player){}
		std::string unit;
		int frame;
		int player;
	};
	struct gamesummary{
		~gamesummary(){unitsdestroyed.clear(); unitsmade.clear();}
		std::vector<data> unitsdestroyed;
		std::vector<data> unitsmade;
	};
	//similar to getsummary but is contained within a time interval and values are in a map.
	struct gamecountinterval{
		gamecountinterval(){}
		gamecountinterval(int low, int high): lowerboundry(low) , higherboundry(high){}
		~gamecountinterval(){unitsmade.clear(); unitsdestroyed.clear();}
		std::map<std::pair<int, std::string>, int> unitsdestroyed;
		std::map<std::pair<int, std::string>, int> unitsmade;
		int lowerboundry, higherboundry;
	};

	gamesummary gameSummary;

	//maybe use some kind of fraction and use Broodwar->getReplayFrameCount(); since this gives the total frames.
	enum gameunits{EARLY = 8000, MID = 12000, LATE = 18000};
	
	//checkvision is used instead of BWAPI onunitshow because it makes text verboose with unhelpful information
	void	checkVision(BWAPI::Unit * unit);
	void	handleVisionEvents();
	void	print(std::string event, BWAPI::Unit * unit);
	gamecountinterval *	summarytointerval(int lowerboundry, int higherboundry);
	
};
