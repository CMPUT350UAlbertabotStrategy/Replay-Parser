#include "kmeans.h"
#include "kmeans.cpp"
using namespace std;

char file[200];
int frame, player, numberofunits;
string unit, unit1, unit2, event, theline;
vector<int> unitsdestroyed;
vector<vector<int> > features;

gamesummary gameSummary;

int main()
{
	//this program requires itself to be located in the same folder as the rep.rgd files
	//==================================
	FILE * pipe= _popen("dir *.rep.rgd /b", "r");//use windows command dir with argument /b for simple file listing add /s for full path
	ifstream thefile("*.rep.rgd"); //empty string to get the first item
	//ifstream should default construct with emtpy string... no single help place 
	while(!feof(pipe))
	{
		fgets(file, 199, pipe);
		cout << file;

		bool flagcreated =false , flagdestroyed =false;
		while(getline(thefile, theline))
		{
			if(theline == "[UNITS LOST]"){ break;}
			istringstream stream(theline);
			//unit race >> namestring >> sometimes they are 3 string names
			stream >> frame >>player>> event >> unit >> unit;
			if(frame !=0 && player != -1){
				if(event == "Created")
				{
					stream >> unit1;
					if(!stream.fail())
					{
						data Event(unit+unit1, frame, player);
						gameSummary.unitsmade.push_back(Event);

						stream >> unit2;
						if(!stream.fail())
						{
							data Event(unit+unit1+unit2, frame, player);
							gameSummary.unitsmade.push_back(Event);
						}
					}
				}
				if(event =="Destroyed")
				{
					stream >> unit1;
					if(!stream.fail())
					{
						data Event(unit+unit1, frame, player);
						gameSummary.unitsdestroyed.push_back(Event);

						if(!stream.fail()){
							stream >> unit2;
							data Event(unit+unit1+unit2, frame, player);
							gameSummary.unitsdestroyed.push_back(Event);

						}
					}

				}

			}
		}

		gamecountinterval * interval = summarytointerval(1, 8000);
		sort(interval->unitsmade);
		gameSummary.unitsdestroyed.clear();
		gameSummary.unitsmade.clear();
		thefile.close();
		thefile.open(file);
	} 
	_pclose(pipe);
	//==================================
}

gamecountinterval * summarytointerval(int lowerboundry, int higherboundry)
{
	gamecountinterval *temp = new gamecountinterval(lowerboundry, higherboundry);
	std::map<std::pair<int, std::string>, int>::iterator it;
	int size = gameSummary.unitsdestroyed.size();
	for(int i =0; i< size; ++i){
		data var = gameSummary.unitsdestroyed[i]; 
		if(var.frame < higherboundry && var.frame >= lowerboundry){
			std::pair<int, std::string> IDandunit = std::make_pair(var.player, var.unit);
			it = temp->unitsdestroyed.find(IDandunit);
			if(it == temp->unitsdestroyed.end())
				temp->unitsdestroyed[IDandunit] = 1;
			else
				++it->second;
		}
	}
	size = gameSummary.unitsmade.size();
	for(int i = 0;i< size;++i){
		data var = gameSummary.unitsmade[i]; 
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
enum Protossfeature{
	Arbiter,Archon,Arbitertribunal,Assimilator,
	Carrier,Corsair,CitadelofAdun,Cyberneticscore,
	Darkarchon,Darktemplar,Dragoon,
	Hightemplar,Interceptor,Observer,Probe,Reaver,
	Scout,Shuttle,Zealot,Gateway,Nexus,
	Observatory,Fleetbeacon,Forge,Photoncannon,
	Pylon,Roboticsfacility,Roboticssupportbay,
	Shieldbattery,Stargate,Templararchives,
};
void sort(std::map<std::pair<int, std::string>, int>)
{

}

