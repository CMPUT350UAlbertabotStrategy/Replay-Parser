#include "ReplayModule.h"

using namespace BWAPI;

std::string filepath;

ReplayModule::ReplayModule(){}
ReplayModule::~ReplayModule(){}
void ReplayModule::print(std::string event, Unit * unit)
{
	this->replayDat << boost::format("%s %s %s %s %s (%s,%s)") 
		%Broodwar->getFrameCount()  %unit->getPlayer()->getID() %event
		%unit->getID() %unit->getType().getName() %unit->getPosition().x()  %unit->getPosition().y()<< std::endl;
}

void ReplayModule::onStart()
{
	Broodwar->setLocalSpeed(0);
	//Broodwar->setFrameSkip(0); //i have no idea what this does
	filepath = Broodwar->mapPathName() + ".rgd";

	this->replayDat.open(filepath.c_str());

	this->replayDat << "[Replay Start]\n" << std::fixed << std::setprecision(4)
		<< "RepPath: " << Broodwar->mapPathName() << std::endl
		<< "MapName: " << Broodwar->mapName() << std::endl
		<< "NumStartPositions: " << Broodwar->getStartLocations().size() << std::endl
		<< "The following players are in this replay:" << std::endl;
	for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
	{
		if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
		{
			int startloc = -1;
			bool foundstart = false;
			for(std::set<TilePosition>::iterator it = Broodwar->getStartLocations().begin(); 
				it != Broodwar->getStartLocations().end() && foundstart == false; it++)
			{
				startloc++;
				if((*p)->getStartLocation() == (*it))
				{
					foundstart = true;
				}
			}
			this->replayDat << (*p)->getID() << ", " << (*p)->getName() << ", " << (*p)->getRace().getName() << ", " << startloc << std::endl;
			this->activePlayers.insert(*p);
		}
	}
	this->replayDat << "Begin replay data:" << std::endl;
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
	this->seenThisTurn.clear();
	this->unseenUnits.clear();
	this->activePlayers.clear();
}
void ReplayModule::checkVision(Unit* u)
{
	for each(Player * p in this->activePlayers)
	{
		if(p != u->getPlayer())
		{
			for each(std::pair<Unit*, UnitType> visionPair in this->unseenUnits[u->getPlayer()])
			{
				Unit* visionTarget = visionPair.first;
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
	for each(Player * p in this->activePlayers)
		for each(Unit* u in p->getUnits())
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

void ReplayModule::onUnitMorph(Unit * unit)
{
	print("Morph", unit);
	data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
	this->gameSummary.unitsmade.push_back(u);
}

void ReplayModule::onUnitCreate(Unit * unit)
{
	print("Created", unit);
	data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
	this->gameSummary.unitsmade.push_back(u);

	TilePosition tp(unit->getPosition());
	if(unit->getType() != UnitTypes::Zerg_Larva)
		for each(Player* p in this->activePlayers)
			if(this->activePlayers.find(unit->getPlayer()) != this->activePlayers.end())
				if(p->getType() != UnitTypes::Zerg_Larva)
					if(p != unit->getPlayer())
						this->unseenUnits[p].insert(std::pair<Unit*, UnitType>(unit, unit->getType()));
}
void ReplayModule::onUnitDestroy(Unit * unit)
{
	print("Destroyed", unit);
	if(unit->getPlayer()->getID() != -1){
		data u (unit->getType().getName(), Broodwar->getFrameCount(), unit->getPlayer()->getID());
		this->gameSummary.unitsdestroyed.push_back(u);
	}
}

void ReplayModule::onUnitRenegade(Unit * unit)
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

void ReplayModule::onFrame()
{
	if(Broodwar->getFrameCount() % 12 == 0)
	{
		handleVisionEvents();
	}

	for each(Player* p in this->activePlayers)
	{
		for each(Unit* u in this->seenThisTurn[p])
		{
			this->unseenUnits[p].erase(std::pair<Unit*, UnitType>(u, u->getType()));
		}
		this->seenThisTurn[p].clear();
	}
}