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
	std::map<BWAPI::Player*, std::set<std::pair<BWAPI::Unit*, BWAPI::UnitType> > > unseenUnits;

	//test datatype
	//first pair is PLAYER ID and UNIT(marine,zealot,etc) then TIME IN SECONDS, NUMBER OF THESE UNITS
	struct data{
		data(){
			for(int i = 0; i<NUMBEROFGAMEUNITS; i++){
				std::map<std::pair<int, std::string>, std::pair<int, int>> m;
				unitsdestroyed.push_back(m);
				std::map<std::pair<int, std::string>, std::pair<int, int>> M;
				unitsmade.push_back(M);
			}
		}
		std::vector<std::map<std::pair<int, std::string>, std::pair<int, int>>> unitsdestroyed; 
		std::vector<std::map<std::pair<int, std::string>, std::pair<int, int>>> unitsmade; 
	};

	data gameSummary;

	enum vectortype{DESTROYEDVECTOR, MADEVECTOR};
	//these game units are an approximate unit of time - ~1 unit/s -
	//ok for machine learning as long as this is kept the same.
	enum gameunits{NUMBEROFGAMEUNITS = 3, EARLY = 240, MID = 540, LATE = 840};
	
	//checkvision is used instead of BWAPI onunitshow because it makes text verboose with unhelpful information
	void	checkVision(BWAPI::Unit * unit);
	void	handleVisionEvents();
	void	intovectors(BWAPI::Unit * unit, UAlbertaBotModule::vectortype);
	void	print(std::string event, BWAPI::Unit * unit);
};
