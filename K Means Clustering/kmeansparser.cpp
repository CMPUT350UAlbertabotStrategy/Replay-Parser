#include "kmeans.h"

using namespace std;

char file[200];
int frame, player, numberofunits;
string unit, unit1, event;
char theline[1000], otherline[1000];
vector<int> unitsdestroyed;
vector<vector<int> > featurelist;
string CAT = "cat ";
gamesummary gameSummary;
bool P0isProtoss, P1isProtoss;

int main(int argc, int ** argv)
{
	//this program requires itself to be located in the same folder as the rep.rgd files
	//==================================
#ifdef linux
	FILE * pipe= _popen("ls *.rep.rgd", "r");//use linux command ls
	cout<< "make sure to have replay files in this directory<< endl;
#elif _WIN32
	FILE * pipe= _popen("dir *.rep.rgd /b", "r");//use windows command dir with argument /b for simple file listing add /s for full path
	cout<< "make sure to have replay files and cat for windows in this directory" << endl;
#endif

	while(!feof(pipe))
	{
		fgets(file, 199, pipe);
		cout << file;

		P0isProtoss =false; P1isProtoss =false;
#ifdef linux
		FILE * cat = popen((CAT + string(file)).c_str(), "r");
#elif _WIN32
		FILE * cat = _popen((CAT + string(file)).c_str(), "r");
#endif
		int currentline = 0;
		while(fgets(theline, 1000, cat))
		{
			//check it end of game summary or no one was protoss
		if(theline == "[UNITS LOST]" ){ break;}}
		if(currentline > 8 && P0isProtoss ==false && P0isProtoss ==false){break;}

		istringstream stream(theline);

		if( currentline < 8){
			string read(theline);
			if(read.find("1,")){
					if(read.find("Protoss")){
						P1isProtoss = true;
					}
			}
				if(read.find("0,")){
					if(read.find("Protoss")){
						P0isProtoss = true;
					}
				}

		}

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
				}
			}
			if(event =="Destroyed")
			{
				stream >> unit1;
				if(!stream.fail())
				{
					data Event(unit+unit1, frame, player);
					gameSummary.unitsdestroyed.push_back(Event);
				}

			}
			++currentline;
		}
		gamecountinterval * interval = summarytointerval(1, 8000);
		sort(interval->unitsmade);
		gameSummary.unitsdestroyed.clear();
		gameSummary.unitsmade.clear();

	}
#ifdef linux
	pclose(pipe);
#elif _WIN32
	_pclose(pipe);
#endif
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
			if(it == temp->unitsmade.end()){
				temp->unitsmade[IDandunit] = 1;}
			else{
				++it->second;}
		}
	}
	return temp;
}

enum Protossfeature{
	Arbiter,Archon,ArbiterTribunal,Assimilator,
	Carrier,Corsair,CitadelofAdun,CyberneticsCore,
	Darkarchon,DarkTemplar,Dragoon,
	HighTemplar,Interceptor,Observer,Probe,Reaver,
	Scout,Shuttle,Zealot,Gateway,Nexus,
	Observatory,FleetBeacon,Forge,Photoncannon,
	Pylon,RoboticsFacility,RoboticsSupportbay,
	Shieldbattery,Stargate,TemplarArchives
};

void sort(std::map<std::pair<int, std::string>, int> summary)
{
	vector<int> features1,features0;
	std::map<std::pair<int, std::string>, int>::iterator it;
	if(P1isProtoss){
		if((it = summary.find(make_pair(1, "Arbiter"))) != summary.end())			features1[Arbiter]= it->second; 
		if((it = summary.find(make_pair(1, "Archon"))) != summary.end())			features1[Archon]= it->second; 
		if((it = summary.find(make_pair(1, "ArbiterTribunal"))) != summary.end())	features1[ArbiterTribunal]= it->second; 
		if((it = summary.find(make_pair(1, "Assimilator"))) != summary.end())		features1[Assimilator]= it->second;
		if((it = summary.find(make_pair(1, "Carrier"))) != summary.end())			features1[Carrier]= it->second; 
		if((it = summary.find(make_pair(1, "Corsair"))) != summary.end())			features1[Corsair]= it->second; 
		if((it = summary.find(make_pair(1, "CitadelofAdun"))) != summary.end())		features1[CitadelofAdun]= it->second; 
		if((it = summary.find(make_pair(1, "CyberneticsCore"))) != summary.end())	features1[CyberneticsCore]= it->second; 
		if((it = summary.find(make_pair(1, "Darkarchon"))) != summary.end())		features1[Darkarchon]= it->second; 
		if((it = summary.find(make_pair(1, "DarkTemplar"))) != summary.end())		features1[DarkTemplar]= it->second; 
		if((it = summary.find(make_pair(1, "Dragoon"))) != summary.end())			features1[Dragoon]= it->second; 
		if((it = summary.find(make_pair(1, "HighTemplar"))) != summary.end())		features1[HighTemplar]= it->second; 
		if((it = summary.find(make_pair(1, "Interceptor"))) != summary.end())		features1[Interceptor]= it->second; 
		if((it = summary.find(make_pair(1, "Observer"))) != summary.end())			features1[Observer]= it->second; 
		if((it = summary.find(make_pair(1, "Probe"))) != summary.end())				features1[Probe]= it->second; 
		if((it = summary.find(make_pair(1, "Reaver"))) != summary.end())			features1[Reaver]= it->second; 
		if((it = summary.find(make_pair(1, "Scout"))) != summary.end())				features1[Scout]= it->second; 
		if((it = summary.find(make_pair(1, "Shuttle"))) != summary.end())			features1[Shuttle]= it->second; 
		if((it = summary.find(make_pair(1, "Zealot"))) != summary.end())			features1[Zealot]= it->second; 
		if((it = summary.find(make_pair(1, "Gateway"))) != summary.end())			features1[Gateway]= it->second; 
		if((it = summary.find(make_pair(1, "Nexus"))) != summary.end())				features1[Nexus]= it->second; 
		if((it = summary.find(make_pair(1, "Observatory"))) != summary.end())		features1[Observatory]= it->second; 
		if((it = summary.find(make_pair(1, "FleetBeacon"))) != summary.end())		features1[FleetBeacon]= it->second; 
		if((it = summary.find(make_pair(1, "Forge"))) != summary.end())				features1[Forge]= it->second; 
		if((it = summary.find(make_pair(1, "Photoncannon"))) != summary.end())		features1[Photoncannon]= it->second; 
		if((it = summary.find(make_pair(1, "Pylon"))) != summary.end())				features1[Pylon]= it->second; 
		if((it = summary.find(make_pair(1, "RoboticsFacility"))) != summary.end())	features1[RoboticsFacility]= it->second; 
		if((it = summary.find(make_pair(1, "RoboticsSupportbay"))) != summary.end())features1[RoboticsSupportbay]= it->second; 
		if((it = summary.find(make_pair(1, "Shieldbattery"))) != summary.end())		features1[Shieldbattery]= it->second;
		if((it = summary.find(make_pair(1, "Stargate"))) != summary.end())			features1[Stargate]= it->second; 
		if((it = summary.find(make_pair(1, "TemplarArchives"))) != summary.end())	features1[TemplarArchives]= it->second; 
		featurelist.push_back(features1);
	}

	if(P0isProtoss){
		if((it = summary.find(make_pair(0, "Arbiter"))) != summary.end())			features0[Arbiter]= it->second; 
		if((it = summary.find(make_pair(0, "Archon"))) != summary.end())			features0[Archon]= it->second; 
		if((it = summary.find(make_pair(0, "ArbiterTribunal"))) != summary.end())	features0[ArbiterTribunal]= it->second; 
		if((it = summary.find(make_pair(0, "Assimilator"))) != summary.end())		features0[Assimilator]= it->second;
		if((it = summary.find(make_pair(0, "Carrier"))) != summary.end())			features0[Carrier]= it->second; 
		if((it = summary.find(make_pair(0, "Corsair"))) != summary.end())			features0[Corsair]= it->second; 
		if((it = summary.find(make_pair(0, "CitadelofAdun"))) != summary.end())		features0[CitadelofAdun]= it->second; 
		if((it = summary.find(make_pair(0, "CyberneticsCore"))) != summary.end())	features0[CyberneticsCore]= it->second; 
		if((it = summary.find(make_pair(0, "Darkarchon"))) != summary.end())		features0[Darkarchon]= it->second; 
		if((it = summary.find(make_pair(0, "DarkTemplar"))) != summary.end())		features0[DarkTemplar]= it->second; 
		if((it = summary.find(make_pair(0, "Dragoon"))) != summary.end())			features0[Dragoon]= it->second; 
		if((it = summary.find(make_pair(0, "HighTemplar"))) != summary.end())		features0[HighTemplar]= it->second; 
		if((it = summary.find(make_pair(0, "Interceptor"))) != summary.end())		features0[Interceptor]= it->second; 
		if((it = summary.find(make_pair(0, "Observer"))) != summary.end())			features0[Observer]= it->second; 
		if((it = summary.find(make_pair(0, "Probe"))) != summary.end())				features0[Probe]= it->second; 
		if((it = summary.find(make_pair(0, "Reaver"))) != summary.end())			features0[Reaver]= it->second; 
		if((it = summary.find(make_pair(0, "Scout"))) != summary.end())				features0[Scout]= it->second; 
		if((it = summary.find(make_pair(0, "Shuttle"))) != summary.end())			features0[Shuttle]= it->second; 
		if((it = summary.find(make_pair(0, "Zealot"))) != summary.end())			features0[Zealot]= it->second; 
		if((it = summary.find(make_pair(0, "Gateway"))) != summary.end())			features0[Gateway]= it->second; 
		if((it = summary.find(make_pair(0, "Nexus"))) != summary.end())				features0[Nexus]= it->second; 
		if((it = summary.find(make_pair(0, "Observatory"))) != summary.end())		features0[Observatory]= it->second; 
		if((it = summary.find(make_pair(0, "FleetBeacon"))) != summary.end())		features0[FleetBeacon]= it->second; 
		if((it = summary.find(make_pair(0, "Forge"))) != summary.end())				features0[Forge]= it->second; 
		if((it = summary.find(make_pair(0, "Photoncannon"))) != summary.end())		features0[Photoncannon]= it->second; 
		if((it = summary.find(make_pair(0, "Pylon"))) != summary.end())				features0[Pylon]= it->second; 
		if((it = summary.find(make_pair(0, "RoboticsFacility"))) != summary.end())	features0[RoboticsFacility]= it->second; 
		if((it = summary.find(make_pair(0, "RoboticsSupportbay"))) != summary.end())features0[RoboticsSupportbay]= it->second; 
		if((it = summary.find(make_pair(0, "Shieldbattery"))) != summary.end())		features0[Shieldbattery]= it->second;
		if((it = summary.find(make_pair(0, "Stargate"))) != summary.end())			features0[Stargate]= it->second; 
		if((it = summary.find(make_pair(0, "TemplarArchives"))) != summary.end())	features0[TemplarArchives]= it->second; 
		featurelist.push_back(features0);
	}
}

