/* 
+----------------------------------------------------------------------+
| UAlbertaBot                                                          |
+----------------------------------------------------------------------+
| University of Alberta - AIIDE StarCraft Competition                  |
+----------------------------------------------------------------------+
|                                                                      |
+----------------------------------------------------------------------+
| Author: David Churchill <dave.churchill@gmail.com>                   |
+----------------------------------------------------------------------+
*/

//TODO make ualbertabot/replay functions more streamlined so compiling doesnt have to happen
//for each type of functionality. 

#include "Common.h"
#include "UAlbertaBotModule.h"

using namespace BWAPI;

BWAPI::AIModule * __NewAIModule()
{
	return new UAlbertaBotModule();
}

UAlbertaBotModule::UAlbertaBotModule()  {}
UAlbertaBotModule::~UAlbertaBotModule() {}

std::string filepath;

void UAlbertaBotModule::onStart()
{
	if(Broodwar->isReplay()){
		BWAPI::Broodwar->setLocalSpeed(0);
		//Broodwar->setFrameSkip(0); //i have no idea what this does
		filepath = BWAPI::Broodwar->mapPathName() + ".rgd";

		replayDat.open(filepath.c_str());

		replayDat << "[Replay Start]\n" << std::fixed << std::setprecision(4)
			<< "RepPath: " << BWAPI::Broodwar->mapPathName() << std::endl
			<< "MapName: " << BWAPI::Broodwar->mapName() << std::endl
			<< "NumStartPositions: " << BWAPI::Broodwar->getStartLocations().size() << std::endl
			<< "The following players are in this replay:" << std::endl;
		for(std::set<BWAPI::Player*>::iterator p=BWAPI::Broodwar->getPlayers().begin();p!=BWAPI::Broodwar->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				int startloc = -1;
				bool foundstart = false;
				for(std::set<BWAPI::TilePosition>::iterator it = BWAPI::Broodwar->getStartLocations().begin(); 
					it != BWAPI::Broodwar->getStartLocations().end() && foundstart == false; it++)
				{
					startloc++;
					if((*p)->getStartLocation() == (*it))
					{
						foundstart = true;
					}
				}
				replayDat << (*p)->getID() << ", " << (*p)->getName() << ", " << (*p)->getRace().getName() << ", " << startloc << std::endl;
				this->activePlayers.insert(*p);
			}
		}
		replayDat << "Begin replay data:" << std::endl;
	}

	if (!Broodwar->isReplay()){
		BWAPI::Broodwar->setLocalSpeed(0);

		BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
		//BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);

		if (Options::Modules::USING_GAMECOMMANDER)
		{
			BWTA::readMap();
			BWTA::analyze();
		}

		if (Options::Modules::USING_MICRO_SEARCH)
		{
			Search::StarcraftData::init();
			MicroSearch::Hash::initHash();
			micro.onStart();
		}

	}
}

void UAlbertaBotModule::onEnd(bool isWinner) 
{
	if (!Broodwar->isReplay()){
		if (Options::Modules::USING_GAMECOMMANDER){

			StrategyManager::Instance().onEnd(isWinner);

			std::stringstream result;
			std::string win = isWinner ? "win" : "lose";

			result << "Game against " << BWAPI::Broodwar->enemy()->getName() << " " << win << 
				" with strategy " << StrategyManager::Instance().getCurrentStrategy() << std::endl;

			Logger::Instance().log(result.str());
		}
	}
	if(Broodwar->isReplay()){
		std::map<std::pair<int, std::string>, int>::iterator it;
		//can be called with any interval
		gamecountinterval  * early, * mid,* late;
		early = summarytointerval(0, EARLY);
		mid = summarytointerval(MID, LATE);
		late = summarytointerval(LATE, Broodwar->getReplayFrameCount());
		this->replayDat<< "[UNITS LOST]" << std::endl;
		for(it = early->unitsdestroyed.begin(); it != early->unitsdestroyed.end();++it){
			this->replayDat << it->first.first << " " << it->first.second << " " << early->lowerboundry<< "-"<< early->higherboundry << " " 
				<< it->second  << std::endl;
		}
		for(it = mid->unitsdestroyed.begin(); it != mid->unitsdestroyed.end();++it){
			this->replayDat << it->first.first << " " << it->first.second << " " << mid->lowerboundry<< "-"<< mid->higherboundry << " " 
				<< it->second  << std::endl;
		}
		for(it = late->unitsdestroyed.begin(); it != late->unitsdestroyed.end();++it){
			this->replayDat << it->first.first << " " << it->first.second << " " << late->lowerboundry<< "-"<< late->higherboundry << " " 
				<< it->second  << std::endl;
		}
		this->replayDat<< "[UNITS CREATED]" << std::endl;
		for(it = early->unitsmade.begin(); it != early->unitsmade.end();++it){
			this->replayDat << it->first.first << " " << it->first.second << " " << early->lowerboundry<< "-"<< early->higherboundry << " " 
				<< it->second  << std::endl;
		}
		for(it = mid->unitsmade.begin(); it != mid->unitsmade.end();++it){
			this->replayDat << it->first.first << " " << it->first.second << " " << mid->lowerboundry<< "-"<< mid->higherboundry << " " 
				<< it->second  << std::endl;
		}
		for(it = late->unitsmade.begin(); it != late->unitsmade.end();++it){
			this->replayDat << it->first.first << " " << it->first.second << " " << late->lowerboundry<< "-"<< late->higherboundry << " " 
				<< it->second  << std::endl;
		}


		this->replayDat << "ELAPSED TIME: "<<Broodwar->elapsedTime() << std::endl << "[EndGame]" << std::endl;
		//clear the vectors in the gamesummary struct, but later I would assume there would be a function processing this data.

		this->replayDat.close();
		this->seenThisTurn.clear();
		this->unseenUnits.clear();
		this->activePlayers.clear();
	}
}

void UAlbertaBotModule::onFrame()
{
	if(Broodwar->isReplay()){
		if(BWAPI::Broodwar->getFrameCount() % 12 == 0)
		{
			handleVisionEvents();
		}

		for each(BWAPI::Player* p in this->activePlayers)
		{
			for each(BWAPI::Unit* u in this->seenThisTurn[p])
			{
				this->unseenUnits[p].erase(std::pair<BWAPI::Unit*, BWAPI::UnitType>(u, u->getType()));
			}
			this->seenThisTurn[p].clear();
		}

	}
	if (!Broodwar->isReplay()){
		if (Options::Modules::USING_GAMECOMMANDER) 
		{ 
			gameCommander.update(); 
		}

		if (Options::Modules::USING_ENHANCED_INTERFACE)
		{
			eui.update();
		}

		if (Options::Modules::USING_MICRO_SEARCH)
		{
			micro.update();
		}

		if (Options::Modules::USING_REPLAY_VISUALIZER)
		{
			BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->getAllUnits())
			{
				BWAPI::Broodwar->drawTextMap(unit->getPosition().x(), unit->getPosition().y(), "   %d", unit->getPlayer()->getID());

				if (unit->isSelected())
				{
					BWAPI::Broodwar->drawCircleMap(unit->getPosition().x(), unit->getPosition().y(), 1000, BWAPI::Colors::Red);
				}
			}
		}
	}
	//Visualizer::Instance().setBWAPIState();
	//Visualizer::Instance().onFrame();
}

void UAlbertaBotModule::onUnitDestroy(BWAPI::Unit * unit)
{
	if(Broodwar->isReplay()){
		print(" Destroyed ", unit);
		if(unit->getPlayer()->getID() != -1){
			UAlbertaBotModule::data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
			this->gameSummary.unitsdestroyed.push_back(u);
		}
	}
	if (!Broodwar->isReplay())
	{
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
		if (Options::Modules::USING_ENHANCED_INTERFACE) { eui.onUnitDestroy(unit); }

	}
}

void UAlbertaBotModule::onUnitMorph(BWAPI::Unit * unit)
{
	if(Broodwar->isReplay()){
		print(" Morph ", unit);
		UAlbertaBotModule::data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
		this->gameSummary.unitsmade.push_back(u);
	}

	if (!Broodwar->isReplay())
	{	
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
	}
}

void UAlbertaBotModule::onUnitCreate(BWAPI::Unit * unit)
{ 
	if(Broodwar->isReplay() && unit->getPlayer()->getID() != -1)
	{
		print(" Created ", unit);
		UAlbertaBotModule::data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
		this->gameSummary.unitsmade.push_back(u);

		BWAPI::TilePosition tp(unit->getPosition());
		if(unit->getType() != BWAPI::UnitTypes::Zerg_Larva)
			for each(BWAPI::Player* p in this->activePlayers)
				if(this->activePlayers.find(unit->getPlayer()) != this->activePlayers.end())
					if(p->getType() != BWAPI::UnitTypes::Zerg_Larva)
						if(p != unit->getPlayer())
							this->unseenUnits[p].insert(std::pair<BWAPI::Unit*, BWAPI::UnitType>(unit, unit->getType()));
	}
	if (Options::Modules::USING_GAMECOMMANDER && !Broodwar->isReplay())
	{
		gameCommander.onUnitCreate(unit); 
	}
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	if (!Broodwar->isReplay())
	{
		BWAPI::Broodwar->sendText(text.c_str());

		if (Options::Modules::USING_REPLAY_VISUALIZER && (text.compare("sim") == 0))
		{
			BWAPI::Unit * selected = NULL;
			BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->getAllUnits())
			{
				if (unit->isSelected())
				{
					selected = unit;
					break;
				}
			}

			if (selected)
			{
#ifdef USING_VISUALIZATION_LIBRARIES
				ReplayVisualizer rv;
				rv.launchSimulation(selected->getPosition(), 1000);
#endif
			}
		}
		else if (text.compare("sim") != 0)
		{
			BWAPI::Broodwar->setLocalSpeed(atoi(text.c_str()));
		}
	}
}

void UAlbertaBotModule::print(std::string event, Unit * unit)
{
	this->replayDat << BWAPI::Broodwar->getFrameCount() << " " << unit->getPlayer()->getID() << event
		<< unit->getID() << " "  << unit->getType().getName() << " (" 
		<< unit->getPosition().x() << " " << unit->getPosition().y() <<")" << std::endl;
}

void UAlbertaBotModule::checkVision(BWAPI::Unit* u)
{
	for each(BWAPI::Player * p in this->activePlayers)
	{
		if(p != u->getPlayer())
		{
			for each(std::pair<BWAPI::Unit*, BWAPI::UnitType> visionPair in this->unseenUnits[u->getPlayer()])
			{
				BWAPI::Unit* visionTarget = visionPair.first;
				int sight = u->getType().sightRange();
				sight = sight * sight;
				int ux = u->getPosition().x();
				int uy = u->getPosition().y();
				int tx = visionTarget->getPosition().x();
				int ty = visionTarget->getPosition().y();
				int dx = ux - tx;
				int dy = uy - ty;
				int dist = (dx * dx) + (dy * dy);
				if(dist <= sight)
				{
					if(this->seenThisTurn[u->getPlayer()].find(visionTarget) == this->seenThisTurn[u->getPlayer()].end())
					{
						print(" Discovered ", u);
						this->seenThisTurn[u->getPlayer()].insert(visionTarget);
					}
				}
			}
		}
	}
}

void UAlbertaBotModule::handleVisionEvents()
{
	for each(BWAPI::Player * p in this->activePlayers)
		for each(BWAPI::Unit* u in p->getUnits())
			checkVision(u);
}

UAlbertaBotModule::gamecountinterval * UAlbertaBotModule::summarytointerval(int lowerboundry, int higherboundry)
{
	gamecountinterval * temp = new gamecountinterval(lowerboundry, higherboundry);
	std::map<std::pair<int, std::string>, int>::iterator it;
	int size = this->gameSummary.unitsdestroyed.size();
	int size2 = this->gameSummary.unitsmade.size();
	for(int i =0; i< size; ++i){
		data var = this->gameSummary.unitsdestroyed[i]; 
		if(var.frame < higherboundry && var.frame >= lowerboundry){
			std::pair<int, std::string> IDandunit = std::make_pair(var.player, var.unit);
			it = temp->unitsdestroyed.find(IDandunit);
			if(it == temp->unitsdestroyed.end())
				temp->unitsdestroyed[IDandunit] = 1;
			else
				it->second++;
		}
		if(i<size2){
			var = this->gameSummary.unitsmade[i]; 
			if(var.frame < higherboundry && var.frame >= lowerboundry){
				std::pair<int, std::string> IDandunit = std::make_pair(var.player, var.unit);
				it = temp->unitsmade.find(IDandunit);
				if(it == temp->unitsmade.end())
					temp->unitsmade[IDandunit] = 1;
				else
					it->second++;

			}
		}
	}

	return temp;
}

void UAlbertaBotModule::onUnitShow(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER && !Broodwar->isReplay()) { gameCommander.onUnitShow(unit); }
}

void UAlbertaBotModule::onUnitHide(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER && !Broodwar->isReplay()) { gameCommander.onUnitHide(unit); }
}

void UAlbertaBotModule::onUnitRenegade(BWAPI::Unit * unit)
{ 
	if(Broodwar->isReplay() && unit->getPlayer()->getID() != -1)
	{
		this->replayDat << Broodwar->getFrameCount() << " " << unit->getPlayer()->getID()
			<< " ChangedOwnership " << unit->getType().getName() << std::endl;
		for each(Player* p in this->activePlayers)
		{
			if(p != unit->getPlayer())
			{
				if(this->activePlayers.find(unit->getPlayer()) != this->activePlayers.end())
				{
					this->unseenUnits[p].insert(std::pair<Unit*, UnitType>(unit, unit->getType()));
				}
			}
			else
			{
				this->unseenUnits[p].erase(std::pair<Unit*, UnitType>(unit, unit->getType()));
			}
		}
	}
	if (!Broodwar->isReplay())
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitRenegade(unit); }
}
