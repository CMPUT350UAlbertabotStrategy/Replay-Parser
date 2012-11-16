#pragma once

#include <D:/downloads/BWAPI 3.7.4/include/BWAPI.h>

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <utility>


DWORD WINAPI AnalyzeThread();


enum AttackType {
	DROP,
	GROUND,
	AIR,
	INVIS
};
class BWRepDump;



class BWRepDump : public BWAPI::AIModule
{

protected:


public:
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onSaveGame(std::string gameName);


	//Replay analysis encoding data
	void handleTechEvents();
	void handleVisionEvents();
	void checkVision(BWAPI::Unit*);

	std::ofstream replayDat;
	std::ofstream replayLocationDat;
	std::ofstream replayOrdersDat;

	
	std::map<BWAPI::Player*, std::list<BWAPI::UpgradeType> > listCurrentlyUpgrading;
	std::map<BWAPI::Player*, std::list<std::pair<BWAPI::UpgradeType, int> > > listUpgraded;
	std::map<BWAPI::Player*, std::list<BWAPI::TechType> > listResearched;
	std::map<BWAPI::Player*, std::list<BWAPI::TechType> > listCurrentlyResearching;
};