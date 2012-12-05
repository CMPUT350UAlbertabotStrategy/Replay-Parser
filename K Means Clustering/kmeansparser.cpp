#include "kmeans.h"

using namespace std;

char file[200], theline[1000];
int frame, player, numberofunits;
string unit, unit1, unit2, event;
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
	FILE * pipe= _popen("find -name \"*.rep.rgd\" -print", "r");//use linux command ls
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
		while(!feof(cat))
		{
			fgets(theline, 999, cat);
			//check it end of game summary or no one was protoss
			//if(theline == "[UNITS LOST]" ){ break;}
			//if(currentline > 8 && P0isProtoss ==false && P0isProtoss ==false){break;}
			istringstream stream(theline);

			//ive changed the start of replays to not include this comma and the move player id and race to the front
			//but these replays i have here do not do that.
			if( currentline < 7 && currentline >4){
				string read(theline);
				if(read.find("1, ") != string::npos){ //newly generated replays would just be "1 protoss" 
					if(read.find("Protoss") != string::npos){
						P1isProtoss = true;
						cout<< read;
					}
				}
				if(read.find("0, ") != string::npos ){//newly generated replays would just be "0 protoss" 
					if(read.find("Protoss") != string::npos){
						P0isProtoss = true;
						cout << read;
					}
				}
			}
			++currentline;
			//unit race >> namestring >> sometimes they are 3 strings that make up a  name
			//Protoss High Templar
			stream >> frame >>player>> event >> unit >> unit >> unit;
			//cout << frame << " " << player<<" "<< event <<" "<< unit << endl;
			if(frame !=0 && player != -1){
				if((player == 0 && P0isProtoss)|| (P1isProtoss && player == 1)){
					if(string(theline).find("Created")  != string::npos)
					{
						stream >> unit1 >> unit2;
						if(unit1.find("(")  != string::npos) //finding a ( character means these are coordinates
						{
							data * Event = new data(unit, frame, player);
							gameSummary.unitsmade.push_back(Event);
							//cout << gameSummary.unitsmade[gameSummary.unitsmade.size()-1]->unit << endl;

						}
						else
						{

							if(unit2.find("(")  != string::npos)
							{
								data * Event = new data(unit+unit1, frame, player);
								gameSummary.unitsmade.push_back(Event);
								//cout << gameSummary.unitsmade[gameSummary.unitsmade.size()-1]->unit << endl;
							}
							else{							

								data * Event = new data(unit+unit1+unit2, frame, player);
								gameSummary.unitsmade.push_back(Event);
								//cout << gameSummary.unitsmade[gameSummary.unitsmade.size()-1]->unit << endl;
							}
						}
					}
				}
			}
		} 

		gamecountinterval * interval = summarytointerval(1, 8000);
		sort(interval->unitsmade);
		gameSummary.unitsmade.clear();
		gameSummary.unitsdestroyed.clear();
		//================================== end of loop ============== all files read ==============
	}
	std::vector<Strategy*>centroids = kmeans(featurelist, 3, 0.1);

	for (unsigned int i = 0; i < centroids.size(); i++)
	{
		std::cout << "centroid " << i << ": ";
		for (unsigned int j = 0; j < centroids[i]->features.size(); j++)
		std::cout << centroids[i]->features[j] << "," << std::endl;
	}
	
#ifdef linux
	pclose(pipe);
#elif _WIN32
	_pclose(pipe);
#endif

}
gamecountinterval * summarytointerval(int lowerboundry, int higherboundry)
{
	gamecountinterval *temp = new gamecountinterval(lowerboundry, higherboundry);
	std::map<std::pair<int, std::string>, int>::iterator it;
	int size = gameSummary.unitsmade.size();
	for(int i = 0;i< size;++i)
	{
		data * var = gameSummary.unitsmade[i];
		if(var->frame < higherboundry && var->frame >= lowerboundry)
		{
			std::pair<int, std::string> IDandunit = std::make_pair(var->player, var->unit);
			it = temp->unitsmade.find(IDandunit);
			if(it == temp->unitsmade.end())
			{
				temp->unitsmade[IDandunit] = 1;
			}
			else
			{
				++it->second;
			}
		}
	}
	/*
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

	for(it = temp->unitsmade.begin(); it!= temp->unitsmade.end(); it++)
	cout<<it->first.second<< " "<<it->second << endl;

	*/


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
		if((it = summary.find(make_pair(1, "Arbiter"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Archon"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "ArbiterTribunal"))) != summary.end())	features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Assimilator"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Carrier"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Corsair"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "CitadelofAdun"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "CyberneticsCore"))) != summary.end())	features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Darkarchon"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "DarkTemplar"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Dragoon"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "HighTemplar"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Interceptor"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Observer"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Probe"))) != summary.end())				features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Reaver"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Scout"))) != summary.end())				features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Shuttle"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Zealot"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Gateway"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Nexus"))) != summary.end())				features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Observatory"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "FleetBeacon"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Forge"))) != summary.end())				features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Photoncannon"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Pylon"))) != summary.end())				features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "RoboticsFacility"))) != summary.end())	features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "RoboticsSupportbay"))) != summary.end())features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Shieldbattery"))) != summary.end())		features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "Stargate"))) != summary.end())			features1.push_back(it->second);else features1.push_back(0);
		if((it = summary.find(make_pair(1, "TemplarArchives"))) != summary.end())	features1.push_back(it->second);else features1.push_back(0);
		featurelist.push_back(features1);
		for(unsigned int i =0; i< features1.size(); i++)
			cout<< features1[i] <<" ";
		cout << endl;
	}

	if(P0isProtoss){
		if((it = summary.find(make_pair(0, "Arbiter"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Archon"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "ArbiterTribunal"))) != summary.end())	features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Assimilator"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Carrier"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Corsair"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "CitadelofAdun"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "CyberneticsCore"))) != summary.end())	features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Darkarchon"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "DarkTemplar"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Dragoon"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "HighTemplar"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Interceptor"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Observer"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Probe"))) != summary.end())				features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Reaver"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Scout"))) != summary.end())				features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Shuttle"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Zealot"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Gateway"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Nexus"))) != summary.end())				features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Observatory"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "FleetBeacon"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Forge"))) != summary.end())				features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Photoncannon"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Pylon"))) != summary.end())				features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "RoboticsFacility"))) != summary.end())	features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "RoboticsSupportbay"))) != summary.end())features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Shieldbattery"))) != summary.end())		features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "Stargate"))) != summary.end())			features0.push_back(it->second);else features0.push_back(0);
		if((it = summary.find(make_pair(0, "TemplarArchives"))) != summary.end())	features0.push_back(it->second);else features0.push_back(0);
		featurelist.push_back(features0);
		for(unsigned int i =0; i< features0.size(); i++)
			cout<< features0[i] << " ";
		cout << endl;
	}
}




