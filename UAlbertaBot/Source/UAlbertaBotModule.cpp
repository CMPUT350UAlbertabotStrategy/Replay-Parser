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

#include "Common.h"
#include "UAlbertaBotModule.h"

using namespace BWAPI;

BWAPI::AIModule * __NewAIModule()
{
	return new UAlbertaBotModule();
}

UAlbertaBotModule::UAlbertaBotModule()  {}
UAlbertaBotModule::~UAlbertaBotModule() {}

void UAlbertaBotModule::onStart()
{
	if (BWAPI::Broodwar->isReplay())
	{
		BWAPI::Broodwar->setLocalSpeed(0);
		//Broodwar->setLatCom(false);
		//Broodwar->setFrameSkip(0);
		std::string filepath = BWAPI::Broodwar->mapPathName() + ".rgd";

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
				for(std::set<BWAPI::TilePosition>::iterator it = BWAPI::Broodwar->getStartLocations().begin(); it != BWAPI::Broodwar->getStartLocations().end() && foundstart == false; it++)
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
		replayDat << "Begin replay data:\n";
	}

	else{
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
	
	if (BWAPI::Broodwar->isReplay()){
		this->replayDat << "[EndGame]" << std::endl;
		this->replayDat.close();
	}
	else if (Options::Modules::USING_GAMECOMMANDER){
		
			StrategyManager::Instance().onEnd(isWinner);

			std::stringstream result;
			std::string win = isWinner ? "win" : "lose";

			result << "Game against " << BWAPI::Broodwar->enemy()->getName() << " " << win << 
				" with strategy " << StrategyManager::Instance().getCurrentStrategy() << std::endl;

			Logger::Instance().log(result.str());
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
	else{
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
		this->replayDat << BWAPI::Broodwar->getFrameCount() << "," << unit->getPlayer()->getID()  << ",Destroyed," 
			<< unit->getID() << "," << unit->getType().getName() << ",(" << unit->getPosition().x() << "," << unit->getPosition().y() <<")" << std::endl;
	}
	else{
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
		if (Options::Modules::USING_ENHANCED_INTERFACE) { eui.onUnitDestroy(unit); }

	}
}

void UAlbertaBotModule::onUnitMorph(BWAPI::Unit * unit)
{
	if(BWAPI::Broodwar->isReplay())
	{
		this->replayDat << BWAPI::Broodwar->getFrameCount() << "," << unit->getPlayer()->getID() << ",Morph,"
			<< unit->getID() << ","  << unit->getType().getName() << ",(" << unit->getPosition().x() << "," << unit->getPosition().y() <<")" << std::endl;
	}
	else{
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
	}
}
void UAlbertaBotModule::onSendText(std::string text) 
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
						//Broodwar->printf("Player %i Discovered Unit: %s [%i]", p->getID(), visionTarget->getType().getName().c_str());
						this->replayDat << BWAPI::Broodwar->getFrameCount() << "," << p->getID() 
							<< ",Discovered," << visionTarget->getID() << "," << visionTarget->getType().getName() << std::endl;
						this->seenThisTurn[u->getPlayer()].insert(visionTarget);
					}
					//this->unseenUnits[u->getPlayer()].erase(std::pair<Unit*, UnitType>(visionTarget, visionTarget->getType()));
					//Vision Event.
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
	if(BWAPI::Broodwar->isReplay())
	{
		//Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
		this->replayDat << BWAPI::Broodwar->getFrameCount() << "," << unit->getPlayer()->getID()  
			<< ",Created," << unit->getID() << "," << unit->getType().getName() << ",(" << unit->getPosition().x() << "," 
			<< unit->getPosition().y() <<")" << std::endl;
		BWAPI::TilePosition tp(unit->getPosition());
		if(unit->getType() != BWAPI::UnitTypes::Zerg_Larva)
		{
			for each(BWAPI::Player* p in this->activePlayers)
			{
				if(this->activePlayers.find(unit->getPlayer()) != this->activePlayers.end())
				{
					if(p->getType() != BWAPI::UnitTypes::Zerg_Larva)
					{
						if(p != unit->getPlayer())
						{
							this->unseenUnits[p].insert(std::pair<BWAPI::Unit*, BWAPI::UnitType>(unit, unit->getType()));
						}
					}
				}
			}
		}
	}
	else if (Options::Modules::USING_GAMECOMMANDER){
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
	if(Broodwar->isReplay())
	{
		this->replayDat << Broodwar->getFrameCount() << "," << unit->getPlayer()->getID() << ",ChangedOwnership," << unit->getID() << "\n";
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
	else if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitRenegade(unit); }
}
