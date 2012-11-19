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
	if (Broodwar->isReplay())
	{
		//const_cast<bool&>(Options::Modules::USING_GAMECOMMANDER) = false;
		for(int i = 0; i<NUMBEROFGAMEUNITS; i++){
			std::map<std::pair<int, std::string>, std::pair<int, int>> m;
			this->gameSummary.unitsdestroyed.push_back(m);
			std::map<std::pair<int, std::string>, std::pair<int, int>> map;
			this->gameSummary.unitsmade.push_back(map);
		}
		BWAPI::Broodwar->setLocalSpeed(0);
		//Broodwar->setFrameSkip(0);
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
	
	if (BWAPI::Broodwar->isReplay())
	{
		//const_cast<bool&>(Options::Modules::USING_GAMECOMMANDER) = true;

		std::map<std::pair<int, std::string>, std::pair<int, int>>::iterator it;
		this->replayDat<< "[UNITS LOST]" << std::endl;
		for(unsigned int i =0; i< gameSummary.unitsdestroyed.size(); i++){
			for (it = this->gameSummary.unitsdestroyed[i].begin();it != this->gameSummary.unitsdestroyed[i].end();it++){
				if(it->first.first != -1){
					this->replayDat <<it->first.first << " " << it->first.second << " " << it->second.first << " " << it->second.second << std::endl;
				}
			}
			gameSummary.unitsdestroyed[i].clear();
		}
		this->replayDat<< "[UNITS CREATED]" << std::endl;
		for(unsigned int i =0; i< gameSummary.unitsmade.size(); i++){
			for (it = this->gameSummary.unitsmade[i].begin();it != this->gameSummary.unitsmade[i].end();it++){
				if(it->first.first != -1){
					this->replayDat <<it->first.first << " " << it->first.second << " " << it->second.first << " " << it->second.second << std::endl;
				}
			}
			gameSummary.unitsmade[i].clear();
		}
		this->replayDat << "ELAPSED TIME: "<<Broodwar->elapsedTime() << std::endl << "[EndGame]" << std::endl;
		this->replayDat.close();
		this->gameSummary.unitsdestroyed.clear();
		this->gameSummary.unitsmade.clear();
		this->seenThisTurn.clear();
		this->unseenUnits.clear();
		this->activePlayers.clear();
	}

}

void UAlbertaBotModule::onFrame()
{
	if(BWAPI::Broodwar->isReplay()){
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
	if(BWAPI::Broodwar->isReplay())
	{
		print(",Destroyed,", unit);
		intovectors(unit, UAlbertaBotModule::DESTROYEDVECTOR);
		
	}
	if (!Broodwar->isReplay())
	{
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
		if (Options::Modules::USING_ENHANCED_INTERFACE) { eui.onUnitDestroy(unit); }

	}
}

void UAlbertaBotModule::onUnitMorph(BWAPI::Unit * unit)
{
	if(BWAPI::Broodwar->isReplay())
	{
		print(",Morph,", unit);
		intovectors(unit, UAlbertaBotModule::MADEVECTOR);
	}

	if (!Broodwar->isReplay())
	{	
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
	}
}
void UAlbertaBotModule::intovectors(BWAPI::Unit * unit, UAlbertaBotModule::vectortype type)
{
		int time = Broodwar->elapsedTime();
		std::pair<int, std::string> theunit(unit->getPlayer()->getID(), unit->getType().getName());

		if(type == UAlbertaBotModule::MADEVECTOR )
		{
			if(time <=EARLY){
				if(this->gameSummary.unitsmade[0].find(theunit) == this->gameSummary.unitsmade[0].end()){
					this->gameSummary.unitsmade[0][theunit] = std::make_pair(EARLY, 1);
				}
				else{
					this->gameSummary.unitsmade[0].find(theunit)->second.second++;
				}
			}
			else if(EARLY < time && time <= MID){
				if(this->gameSummary.unitsmade[1].find(theunit) == this->gameSummary.unitsmade[1].end()){
					this->gameSummary.unitsmade[1][theunit] = std::make_pair(MID, 1);
				}
				else{
					this->gameSummary.unitsmade[1].find(theunit)->second.second++;
				}
			}
			else if(time > LATE){
				if(this->gameSummary.unitsmade[2].find(theunit) == this->gameSummary.unitsmade[2].end()){
					this->gameSummary.unitsmade[2][theunit] = std::make_pair(LATE, 1);
				}
				else{
					this->gameSummary.unitsmade[2].find(theunit)->second.second++;
				}
			}
		}
		if(type == UAlbertaBotModule::DESTROYEDVECTOR )
		{
			if(time <=EARLY){
				if(this->gameSummary.unitsdestroyed[0].find(theunit) == this->gameSummary.unitsdestroyed[0].end()){
					this->gameSummary.unitsdestroyed[0][theunit] = std::make_pair(EARLY, 1);
				}
				else{
					this->gameSummary.unitsdestroyed[0].find(theunit)->second.second++;
				}
			}
			else if(EARLY < time && time <= MID){
				if(this->gameSummary.unitsdestroyed[1].find(theunit) == this->gameSummary.unitsdestroyed[1].end())
					this->gameSummary.unitsdestroyed[1][theunit] = std::make_pair(MID, 1);
				else{
					this->gameSummary.unitsdestroyed[1].find(theunit)->second.second++;
				}
			}
			else if(time > LATE){
				if(this->gameSummary.unitsdestroyed[2].find(theunit) == this->gameSummary.unitsdestroyed[2].end()){
					this->gameSummary.unitsdestroyed[2][theunit] = std::make_pair(LATE, 1);
				}
				else{
					this->gameSummary.unitsdestroyed[2].find(theunit)->second.second++;
				}
			}
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
	this->replayDat << BWAPI::Broodwar->elapsedTime() << "," << unit->getPlayer()->getID() << event
		<< unit->getID() << ","  << unit->getType().getName() << ",(" 
		<< unit->getPosition().x() << "," << unit->getPosition().y() <<")" << std::endl;
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
							print(",Discovered,", u);
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
	{
		for each(BWAPI::Unit* u in p->getUnits())
		{
			checkVision(u);
		}
	}
}


void UAlbertaBotModule::onUnitCreate(BWAPI::Unit * unit)
{ 
	if(BWAPI::Broodwar->isReplay() && unit->getPlayer()->getID() != -1)
	{
		print(",Created,", unit);
		intovectors(unit, UAlbertaBotModule::MADEVECTOR);

		BWAPI::TilePosition tp(unit->getPosition());
		if(unit->getType() != BWAPI::UnitTypes::Zerg_Larva){
			for each(BWAPI::Player* p in this->activePlayers){
				if(this->activePlayers.find(unit->getPlayer()) != this->activePlayers.end()){
					if(p->getType() != BWAPI::UnitTypes::Zerg_Larva){
						if(p != unit->getPlayer()){
							this->unseenUnits[p].insert(std::pair<BWAPI::Unit*, BWAPI::UnitType>(unit, unit->getType()));
						}
					}
				}
			}
		}
	}
	if (Options::Modules::USING_GAMECOMMANDER && !Broodwar->isReplay())
	{
		  gameCommander.onUnitCreate(unit); 
	}
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
		this->replayDat << Broodwar->elapsedTime() << "," << unit->getPlayer()->getID()
		<< ",ChangedOwnership," << unit->getType().getName() << std::endl;
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
