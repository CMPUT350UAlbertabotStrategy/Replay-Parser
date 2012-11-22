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

BWAPI::AIModule * __NewAIModule()
{
	return new UAlbertaBotModule();
}

UAlbertaBotModule::UAlbertaBotModule()  {}
UAlbertaBotModule::~UAlbertaBotModule() {}

void UAlbertaBotModule::onStart()
{
	if(BWAPI::Broodwar->isReplay()){
		replay.onStart();
	}

	if (!BWAPI::Broodwar->isReplay()){
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
	if (!BWAPI::Broodwar->isReplay()){
		if (Options::Modules::USING_GAMECOMMANDER){

			StrategyManager::Instance().onEnd(isWinner);

			std::stringstream result;
			std::string win = isWinner ? "win" : "lose";

			result << "Game against " << BWAPI::Broodwar->enemy()->getName() << " " << win << 
				" with strategy " << StrategyManager::Instance().getCurrentStrategy() << std::endl;

			Logger::Instance().log(result.str());
		}
	}
	if(BWAPI::Broodwar->isReplay()){
		replay.onEnd(isWinner);
	}
}

void UAlbertaBotModule::onFrame()
{
	if(BWAPI::Broodwar->isReplay()){
		replay.onFrame();
	}
	if (!BWAPI::Broodwar->isReplay()){
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
	if(BWAPI::Broodwar->isReplay()){
		replay.onUnitDestroy(unit);
	}
	if (!BWAPI::Broodwar->isReplay())
	{
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
		if (Options::Modules::USING_ENHANCED_INTERFACE) { eui.onUnitDestroy(unit); }

	}
}

void UAlbertaBotModule::onUnitMorph(BWAPI::Unit * unit)
{
	if(BWAPI::Broodwar->isReplay()){
		replay.onUnitMorph(unit);
	}

	if (!BWAPI::Broodwar->isReplay())
	{	
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
	}
}

void UAlbertaBotModule::onUnitCreate(BWAPI::Unit * unit)
{ 
	if(BWAPI::Broodwar->isReplay() && unit->getPlayer()->getID() != -1){
		replay.onUnitCreate(unit);
	}
	if (Options::Modules::USING_GAMECOMMANDER && !BWAPI::Broodwar->isReplay())
	{
		gameCommander.onUnitCreate(unit); 
	}
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	if (!BWAPI::Broodwar->isReplay())
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


void UAlbertaBotModule::onUnitShow(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER && !BWAPI::Broodwar->isReplay()) { gameCommander.onUnitShow(unit); }
}

void UAlbertaBotModule::onUnitHide(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER && !BWAPI::Broodwar->isReplay()) { gameCommander.onUnitHide(unit); }
}

void UAlbertaBotModule::onUnitRenegade(BWAPI::Unit * unit)
{ 
	if(BWAPI::Broodwar->isReplay() && unit->getPlayer()->getID() != -1)
	{
		replay.onUnitRenegade(unit);
	}
	if (!BWAPI::Broodwar->isReplay())
		if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitRenegade(unit); }
}
