#include "ReplayModule.h"

using namespace BWAPI;

void ReplayModule::print(std::string event, Unit * unit)
{
	this->replayDat << boost::format("%s %s %s %s %s (%s,%s)") 
		%BWAPI::Broodwar->getFrameCount()  %unit->getPlayer()->getID() %event
		%unit->getID() %unit->getType().getName() %unit->getPosition().x()  %unit->getPosition().y()<< std::endl;
}

void ReplayModule::onStart()
{
		BWAPI::Broodwar->setLocalSpeed(0);
		//Broodwar->setFrameSkip(0); //i have no idea what this does
		std::string filepath = BWAPI::Broodwar->mapPathName() + ".rgd";

		replayDat.open(filepath.c_str());

		replayDat << "[Replay Start]"<< std::endl << std::fixed << std::setprecision(4)
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
				replayDat << (*p)->getID() << " " << (*p)->getRace().getName() << " "<< (*p)->getName() << " "  << startloc << std::endl;
				this->activePlayers.insert(*p);
			}
		}
		replayDat << "Begin replay data:" << std::endl;
}

void ReplayModule::checkVision(BWAPI::Unit* u)
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
						print("Discovered", u);
						this->seenThisTurn[u->getPlayer()].insert(visionTarget);
					}
				}
			}
		}
	}
}

void ReplayModule::handleVisionEvents()
{
	for each(BWAPI::Player * p in this->activePlayers)
		for each(BWAPI::Unit* u in p->getUnits())
			checkVision(u);
}

ReplayModule::gamecountinterval * ReplayModule::summarytointerval(int lowerboundry, int higherboundry)
{
	gamecountinterval *temp = new gamecountinterval(lowerboundry, higherboundry);
	std::map<std::pair<int, std::string>, int>::iterator it;
	int size = this->gameSummary.unitsdestroyed.size();
	for(int i =0; i< size; ++i){
		data var = this->gameSummary.unitsdestroyed[i]; 
		if(var.frame < higherboundry && var.frame >= lowerboundry){
			std::pair<int, std::string> IDandunit = std::make_pair(var.player, var.unit);
			it = temp->unitsdestroyed.find(IDandunit);
			if(it == temp->unitsdestroyed.end())
				temp->unitsdestroyed[IDandunit] = 1;
			else
				++it->second;
		}
	}
	size = this->gameSummary.unitsmade.size();
	for(int i = 0;i< size;++i){
		data var = this->gameSummary.unitsmade[i]; 
		if(var.frame < higherboundry && var.frame >= lowerboundry){
			std::pair<int, std::string> IDandunit = std::make_pair(var.player, var.unit);
			it = temp->unitsmade.find(IDandunit);
			if(it == temp->unitsmade.end())
				temp->unitsmade[IDandunit] = 1;
			else
				++it->second;

		}
	}


	return temp;
}
void ReplayModule::onUnitMorph(BWAPI::Unit * unit){
		print("Morph", unit);
		data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
		this->gameSummary.unitsmade.push_back(u);
}
void ReplayModule::onUnitRenegade(BWAPI::Unit * unit)
{ 
		this->replayDat << boost::format("%s %s %s %s") %Broodwar->getFrameCount() %unit->getPlayer()->getID()
			%"ChangedOwnership" %unit->getType().getName() << std::endl;
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

void ReplayModule::onUnitCreate(BWAPI::Unit * unit)
{
		print("Created", unit);
		ReplayModule::data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
		this->gameSummary.unitsmade.push_back(u);

		BWAPI::TilePosition tp(unit->getPosition());
		if(unit->getType() != BWAPI::UnitTypes::Zerg_Larva)
			for each(BWAPI::Player* p in this->activePlayers)
				if(this->activePlayers.find(unit->getPlayer()) != this->activePlayers.end())
					if(p->getType() != BWAPI::UnitTypes::Zerg_Larva)
						if(p != unit->getPlayer())
							this->unseenUnits[p].insert(std::pair<BWAPI::Unit*, BWAPI::UnitType>(unit, unit->getType()));
}

void ReplayModule::onUnitDestroy(BWAPI::Unit * unit)
{
		print("Destroyed", unit);
		if(unit->getPlayer()->getID() != -1){
			ReplayModule::data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
			this->gameSummary.unitsdestroyed.push_back(u);
		}
}

void ReplayModule::onEnd(bool isWinner) {
		this->replayDat << "[ELAPSED TIME]: "<<Broodwar->elapsedTime() << std::endl; 
		std::map<std::pair<int, std::string>, int>::iterator it;
		//can be called with any interval
		gamecountinterval  * early, * mid,* late;
		early = summarytointerval(0, EARLY);
		mid = summarytointerval(MID, LATE);
		late = summarytointerval(LATE, Broodwar->getReplayFrameCount());
		//pairs are nice because they can get sorted
		this->replayDat<< "[UNITS LOST]" << std::endl;
		for(it = early->unitsdestroyed.begin(); it != early->unitsdestroyed.end();++it){
			this->replayDat << boost::format("%s %s %s [%s-%s]") %it->first.first %it->first.second 
				 %it->second %early->lowerboundry %early->higherboundry << std::endl;
		}
		for(it = mid->unitsdestroyed.begin(); it != mid->unitsdestroyed.end();++it){
			this->replayDat << boost::format("%s %s %s [%s-%s]") %it->first.first %it->first.second 
				 %it->second %mid->lowerboundry %mid->higherboundry << std::endl;
		}
		for(it = late->unitsdestroyed.begin(); it != late->unitsdestroyed.end();++it){
			this->replayDat << boost::format("%s %s %s [%s-%s]") %it->first.first %it->first.second 
				 %it->second %late->lowerboundry %late->higherboundry << std::endl;
		}
		this->replayDat<< "[UNITS CREATED]" << std::endl;
		for(it = early->unitsmade.begin(); it != early->unitsmade.end();++it){
			this->replayDat << boost::format("%s %s %s [%s-%s]") %it->first.first %it->first.second 
				 %it->second %early->lowerboundry %early->higherboundry << std::endl;
		}
		for(it = mid->unitsmade.begin(); it != mid->unitsmade.end();++it){
			this->replayDat << boost::format("%s %s %s [%s-%s]") %it->first.first %it->first.second 
				 %it->second %mid->lowerboundry %mid->higherboundry << std::endl;
		}
		for(it = late->unitsmade.begin(); it != late->unitsmade.end();++it){
			this->replayDat << boost::format("%s %s %s [%s-%s]") %it->first.first %it->first.second 
				 %it->second %late->lowerboundry %late->higherboundry << std::endl;
		}
	
		this->replayDat << "[EndGame]" << std::endl;
		//clear the vectors in the gamesummary struct, but later I would assume there would be a function processing this data.

		this->replayDat.close();
}

void ReplayModule::onFrame(){

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

