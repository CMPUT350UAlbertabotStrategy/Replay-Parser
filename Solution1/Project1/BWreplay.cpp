#include "BWreplay.h"
#include <sstream>
#include <iomanip>

using namespace BWAPI;


/* Return TRUE if file 'fileName' exists */
bool fileExists(const char *fileName)
{
    DWORD fileAttr;
    fileAttr = GetFileAttributesA(fileName);
    if (0xFFFFFFFF == fileAttr)
        return false;
    return true;
}

std::string convertInt(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

int hash(const BWAPI::TilePosition& p)
{
	return (((p.x() + 1) << 16) | p.y());
}


void BWRepDump::onStart()
{
	// Enable some cheat flags
	//Broodwar->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	//read map information into BWTA so terrain analysis can be done in another thread

	if (Broodwar->isReplay())
	{
		Broodwar->setLocalSpeed(0);
		//Broodwar->setLatCom(false);
		//Broodwar->setFrameSkip(0);
		std::ofstream myfile;
		std::string filepath = Broodwar->mapPathName() + ".rgd";
		std::string locationfilepath = Broodwar->mapPathName() + ".rld";
		std::string ordersfilepath = Broodwar->mapPathName() + ".rod";
		replayDat.open(filepath.c_str());

		std::string tmpColumns("Regions,");

		replayDat << "[Replay Start]\n" << std::fixed << std::setprecision(4);
		//myfile.close();
		Broodwar->printf("RepPath: %s", Broodwar->mapPathName().c_str());
		replayDat << "RepPath: " << Broodwar->mapPathName() << "\n"; 
		Broodwar->printf("MapName: %s", Broodwar->mapName().c_str());
		replayDat << "MapName: " << Broodwar->mapName() << "\n";
		Broodwar->printf("NumStartPositions: %d", Broodwar->getStartLocations().size());
		replayDat << "NumStartPositions: " << Broodwar->getStartLocations().size() << "\n";
		Broodwar->printf("The following players are in this replay:");
		replayDat << "The following players are in this replay:\n";
		for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				int startloc = -1;
				bool foundstart = false;
				for(std::set<TilePosition>::iterator it = Broodwar->getStartLocations().begin(); it != Broodwar->getStartLocations().end() && foundstart == false; it++)
				{
					startloc++;
					if((*p)->getStartLocation() == (*it))
			  {
				  foundstart = true;
			  }
				}

				Broodwar->printf("%s, %s, %d",(*p)->getName().c_str(),(*p)->getRace().getName().c_str(), startloc);
				replayDat << (*p)->getID() << ", " << (*p)->getName() << ", " << (*p)->getRace().getName() << ", " << startloc << "\n";
				this->activePlayers.insert(*p);
			}
		}
		replayDat << "Begin replay data:\n";
	}
}

void BWRepDump::onEnd(bool isWinner)
{
	for (std::list<attack>::iterator it = attacks.begin();
		it != attacks.end(); )
	{
		endAttack(it, NULL, NULL);
		attacks.erase(it++);
	}
	this->replayDat << "[EndGame]\n";
	this->replayDat.close();

	this->replayOrdersDat.close();
    delete [] _lowResWalkability;
	if (isWinner)
	{
		//log win to file
	}
}

void BWRepDump::checkVision(Unit* u)
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
						//Broodwar->printf("Player %i Discovered Unit: %s [%i]", p->getID(), visionTarget->getType().getName().c_str());
						this->replayDat << Broodwar->getFrameCount() << "," << p->getID() << ",Discovered," << visionTarget->getID() << "," << visionTarget->getType().getName() << "\n";
						this->seenThisTurn[u->getPlayer()].insert(visionTarget);
					}
					//this->unseenUnits[u->getPlayer()].erase(std::pair<Unit*, UnitType>(visionTarget, visionTarget->getType()));
					//Vision Event.
				}
			}
		}
	}
}

void BWRepDump::handleTechEvents()
{
	for each(Player * p in this->activePlayers)
	{
		std::map<Player*, std::list<TechType>>::iterator currentTechIt = this->listCurrentlyResearching.find(p);
		for each (BWAPI::TechType currentResearching in BWAPI::TechTypes::allTechTypes())
		{
			std::list<TechType>* techListPtr;
			if(currentTechIt != this->listCurrentlyResearching.end())
			{
				techListPtr = &((*currentTechIt).second);
			}
			else
			{
				this->listCurrentlyResearching[p] = std::list<TechType>();
				techListPtr = &this->listCurrentlyResearching[p];
			}
			std::list<TechType> techList = (*techListPtr);

			bool wasResearching = false;
			for each (BWAPI::TechType lastFrameResearching in techList)
			{
				if(lastFrameResearching.getID() == currentResearching.getID())
				{
					wasResearching = true;
					break;
				}
			}
			if(p->isResearching(currentResearching))
			{
				if(!wasResearching)
				{
					this->listCurrentlyResearching[p].push_back(currentResearching);
					this->replayDat << Broodwar->getFrameCount() << "," << p->getID() << ",StartResearch," << currentResearching.getName() << "\n";
					//Event - researching new tech
				}
			}
			else
			{
				if(wasResearching)
				{
					if(p->hasResearched(currentResearching))
					{
						this->replayDat << Broodwar->getFrameCount() << "," << p->getID() << ",FinishResearch," << currentResearching.getName() << "\n";
						if(this->listResearched.count(p) > 0)
						{
							this->listResearched[p].push_back(currentResearching);
						}
						this->listCurrentlyResearching[p].remove(currentResearching);
						//Event - research complete
					}
					else
					{
						this->replayDat << Broodwar->getFrameCount() << "," << p->getID() << ",CancelResearch," << currentResearching.getName() << "\n";
						this->listCurrentlyResearching[p].remove(currentResearching);
						//Event - research cancelled
					}
				}
			}

		}
		std::map<Player*, std::list<UpgradeType>>::iterator currentUpgradeIt = this->listCurrentlyUpgrading.find(p);
		for each (BWAPI::UpgradeType checkedUpgrade in BWAPI::UpgradeTypes::allUpgradeTypes())
		{
			std::list<UpgradeType>* upgradeListPtr;
			if(currentUpgradeIt != this->listCurrentlyUpgrading.end())
			{
				upgradeListPtr = &((*currentUpgradeIt).second);
			}
			else
			{
				this->listCurrentlyUpgrading[p] = std::list<UpgradeType>();
				upgradeListPtr = &this->listCurrentlyUpgrading[p];
			}
			std::list<UpgradeType> upgradeList = (*upgradeListPtr);


			bool wasResearching = false;
			for each (BWAPI::UpgradeType lastFrameUpgrading in upgradeList)
			{
				if(lastFrameUpgrading.getID() == checkedUpgrade.getID())
				{
					wasResearching = true;
					break;
				}
			}
			if(p->isUpgrading(checkedUpgrade))
			{
				if(!wasResearching)
				{
					this->listCurrentlyUpgrading[p].push_back(checkedUpgrade);
					this->replayDat << Broodwar->getFrameCount() << "," << p->getID() << ",StartUpgrade," << checkedUpgrade.getName() << "," << (p->getUpgradeLevel(checkedUpgrade) + 1) << "\n";
					//Event - researching new upgrade
				}
			}
			else
			{
				if(wasResearching)
				{
					int lastlevel = 0;
					for each (std::pair<UpgradeType, int> upgradePair in this->listUpgraded[p])
					{
						if(upgradePair.first == checkedUpgrade && upgradePair.second > lastlevel)
						{
							lastlevel = upgradePair.second;
						}
					}
					if(p->getUpgradeLevel(checkedUpgrade) > lastlevel)
					{
						this->replayDat << Broodwar->getFrameCount() << "," << p->getID() << ",FinishUpgrade," << checkedUpgrade.getName() << "," << p->getUpgradeLevel(checkedUpgrade) << "\n";
						if(this->listUpgraded.count(p) > 0)
						{
							this->listUpgraded[p].push_back(std::pair<UpgradeType, int>(checkedUpgrade, p->getUpgradeLevel(checkedUpgrade)));
						}
						this->listCurrentlyUpgrading[p].remove(checkedUpgrade);
						//Event - upgrade complete
					}
					else
					{
						this->replayDat << Broodwar->getFrameCount() << "," << p->getID() << ",CancelUpgrade," << checkedUpgrade.getName() << "," << (p->getUpgradeLevel(checkedUpgrade) + 1) << "\n";
						this->listCurrentlyUpgrading[p].remove(checkedUpgrade);
						//Event - upgrade cancelled
					}
				}
			}
		}
	}
}

void BWRepDump::onReceiveText(BWAPI::Player* player, std::string text)
{
	if(Broodwar->isReplay())
	{
		//Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
		this->replayDat << Broodwar->getFrameCount() << "," << player->getID() << ",SendMessage," << text << "\n";
	}
}

void BWRepDump::onPlayerLeft(BWAPI::Player* player)
{
	if(Broodwar->isReplay())
	{
		//Broodwar->sendText("%s left the game.",player->getName().c_str());
		this->replayDat << Broodwar->getFrameCount() << "," << player->getID() << ",PlayerLeftGame\n";
	}
}

void BWRepDump::onNukeDetect(BWAPI::Position target)
{
	/*
	if (target!=Positions::Unknown)
	Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
	else
	Broodwar->printf("Nuclear Launch Detected");
	*/
	if(Broodwar->isReplay())
	{
		this->replayDat << Broodwar->getFrameCount() << "," << (-1) << ",NuclearLaunch,(" << target.x() << target.y() << "\n";
	}
}

void BWRepDump::onUnitCreate(BWAPI::Unit* unit)
{
	BWAPI::Position p = unit->getPosition();
	this->unitPositionMap[unit] = p;
	BWAPI::TilePosition tp = unit->getTilePosition();
	this->unitCDR[unit] = rd.chokeDependantRegion[tp.x()][tp.y()];
	this->unitRegion[unit] = BWTA::getRegion(p);
	/*
	if (Broodwar->getFrameCount()>1)
	{

	if (!Broodwar->isReplay())
	Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
	if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
	{
	int seconds=Broodwar->getFrameCount()/24;
	int minutes=seconds/60;
	seconds%=60;
	Broodwar->sendText("%.2d:%.2d: %s creates a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
	}
	}

	Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	}
	*/
	if(Broodwar->isReplay())
	{
		//Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
		this->replayDat << Broodwar->getFrameCount() << "," << unit->getPlayer()->getID()  << ",Created," << unit->getID() << "," 
			<< unit->getType().getName() << ",(" << unit->getPosition().x() << "," << unit->getPosition().y() <<")";
	}
}

void BWRepDump::onUnitDestroy(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
	{
	}
	//Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
		updateAggroPlayers(unit);

		//Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
		this->replayDat << Broodwar->getFrameCount() << "," << unit->getPlayer()->getID()  << ",Destroyed," 
			<< unit->getID() << "," << unit->getType().getName() << ",(" << unit->getPosition().x() << "," << unit->getPosition().y() <<")\n";
}
}

void BWRepDump::onUnitMorph(BWAPI::Unit* unit)
{
	/*
	if (!Broodwar->isReplay())
	Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
	if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
	{
	int seconds=Broodwar->getFrameCount()/24;
	int minutes=seconds/60;
	seconds%=60;
	Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
	}
	}
	*/
	if(Broodwar->isReplay())
	{
		//Broodwar->printf("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
		this->replayDat << Broodwar->getFrameCount() << "," << unit->getPlayer()->getID() << ",Morph,"
			<< unit->getID() << ","  << unit->getType().getName() << ",(" << unit->getPosition().x() << "," << unit->getPosition().y() <<")\n";

	}
}

void BWRepDump::onUnitRenegade(BWAPI::Unit* unit)
{
	//if (Broodwar->isReplay())
	//Broodwar->printf("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
	if(Broodwar->isReplay())
	{
		this->replayDat << Broodwar->getFrameCount() << "," << unit->getPlayer()->getID() << ",ChangedOwnership," << unit->getID() << "\n";

	}
}
