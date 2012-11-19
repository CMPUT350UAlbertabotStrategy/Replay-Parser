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

#include <map>

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

	//new functions and variables
	std::ofstream replayDat;
	std::map<BWAPI::Player*, std::set<BWAPI::Unit*> > seenThisTurn;
	std::set<BWAPI::Player*> activePlayers;
	std::map<BWAPI::Player*, std::set<std::pair<BWAPI::Unit*, BWAPI::UnitType> > > unseenUnits;

	//test datatype
	//each unit knows who owns it

	std::map<std::pair<int, std::string>, int> unitsdestroyed; 
	std::map<std::pair<int, std::string>, int> unitsmade; 
	void	checkVision(BWAPI::Unit * unit);
	void	handleVisionEvents();
};
