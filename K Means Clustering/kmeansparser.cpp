#include "kmeans.h"

using namespace std;

char file[200], theline[1000];
int frame, player, numberofunits;
string unit, unit1, unit2, event;
vector<vector<double>> featurelistMINERALS, featurelist;
gamesummary gameSummary;
bool P0isProtoss, P1isProtoss;

int main(int argc, int ** argv)
{
	//this program requires itself to be located in the same folder as the rep.rgd files
	//==================================
#ifdef linux
	FILE * pipe= popen("find -name \"*.rep.rgd\" -print", "r");//use linux command ls
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
		FILE * cat = popen(("cat " + string(file)).c_str(), "r");
#elif _WIN32
		FILE * cat = _popen(("cat " + string(file)).c_str(), "r");
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
				if(read.find("1, ") != string::npos){
					if(read.find("Protoss") != string::npos){
						P1isProtoss = true;
						cout<< read;
					}
				}
				if(read.find("0, ") != string::npos ){
					if(read.find("Protoss") != string::npos){
						P0isProtoss = true;
						cout << read;
					}
				}
			}
			++currentline;
			//unit race >> namestring >> sometimes they are 3 strings that make up a  name
			//something like Protoss High Templar
			stream >> frame >>player>> event >> unit >> unit >> unit;
			//cout << frame << " " << player<<" "<< event <<" "<< unit << endl;
			if(frame !=0 && player != -1){
				if((player == 0 && P0isProtoss)|| (P1isProtoss && player == 1)){
					if(string(theline).find("Created")  != string::npos || string(theline).find("Morph")  != string::npos)
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
		protossfeatureV(interval->unitsmade);
		gameSummary.unitsmade.clear();
		gameSummary.unitsdestroyed.clear();
		//================================== end of loop ============== all files read ==============
	}
	std::vector<Strategy*>centroids = kmeans(featurelist, 3, 0.1);
	unsigned int size = centroids.size();
	for (unsigned int i = 0; i < size; i++)
	{
		std::cout << "centroid " << i << ": ";
		for (unsigned int j = 0; j < centroids[i]->features.size(); j++)
			std::cout << centroids[i]->features[j] << "," ;

		cout << std::endl;
	}
	//for(int i =3; i<7; i++){
		//centroids = kmeans(featurelistMINERALS, i, 0.1);
	centroids = kmeans(featurelistMINERALS, 3, 0.1);
	size = centroids.size();
	for (unsigned int i = 0; i < size; i++)
	{
		std::cout << "centroid " << i << ": ";
		for (unsigned int j = 0; j < centroids[i]->features.size(); j++)
			std::cout << centroids[i]->features[j] << "," ;

		cout << std::endl;
	}
	//}
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
	if(it == temp->unitsdestroyed.end()){
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
	Arbiter=100,Archon=100,ArbiterTribunal=200,Assimilator=100,
	Carrier=350,Corsair=150,CitadelofAdun=150,CyberneticsCore=200,
	Darkarchon=250,DarkTemplar=125,Dragoon=125,
	HighTemplar=50,Interceptor=25,Observer=25,Probe=50,Reaver=275,
	Scout= 275,Shuttle = 200,Zealot=100,Gateway=150,Nexus=400,
	Observatory=50,FleetBeacon=300,Forge=150,Photoncannon=150,
	Pylon=100,RoboticsFacility=200,RoboticsSupportbay=150,
	Shieldbattery=100,Stargate=150,TemplarArchives=150
};

void protossfeatureV(std::map<std::pair<int, std::string>, int> summary)
{
	vector<double> features1withminerals, features1 ,features0, features0withminerals;
	std::map<std::pair<int, std::string>, int>::iterator it;
	if(P1isProtoss){
		if((it = summary.find(make_pair(1, "Arbiter"))) != summary.end()){			
			features1withminerals.push_back(it->second*Arbiter); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Archon"))) != summary.end()){			
			features1withminerals.push_back(it->second*Archon); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "ArbiterTribunal"))) != summary.end()){	
			features1withminerals.push_back(it->second*ArbiterTribunal); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Assimilator"))) != summary.end()){		
			features1withminerals.push_back(it->second*Assimilator); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Carrier"))) != summary.end()){			
			features1withminerals.push_back(it->second*Carrier); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Corsair"))) != summary.end()){			
			features1withminerals.push_back(it->second*Corsair); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "CitadelofAdun"))) != summary.end()){		
			features1withminerals.push_back(it->second*CitadelofAdun); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "CyberneticsCore"))) != summary.end()){	
			features1withminerals.push_back(it->second*CyberneticsCore); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "DarkArchon"))) != summary.end()){		
			features1withminerals.push_back(it->second*Darkarchon); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "DarkTemplar"))) != summary.end()){		
			features1withminerals.push_back(it->second*DarkTemplar); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Dragoon"))) != summary.end()){			
			features1withminerals.push_back(it->second*Dragoon); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "HighTemplar"))) != summary.end()){		
			features1withminerals.push_back(it->second*HighTemplar); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Interceptor"))) != summary.end()){		
			features1withminerals.push_back(it->second*Interceptor); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Observer"))) != summary.end()){			
			features1withminerals.push_back(it->second*Observer); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Probe"))) != summary.end()){				
			features1withminerals.push_back(it->second*Probe); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Reaver"))) != summary.end()){			
			features1withminerals.push_back(it->second*Reaver); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Scout"))) != summary.end()){				
			features1withminerals.push_back(it->second*Scout); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Shuttle"))) != summary.end()){			
			features1withminerals.push_back(it->second*Shuttle); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Zealot"))) != summary.end()){			
			features1withminerals.push_back(it->second*Zealot); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Gateway"))) != summary.end()){			
			features1withminerals.push_back(it->second*Gateway); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Nexus"))) != summary.end()){				
			features1withminerals.push_back(it->second*Nexus); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Observatory"))) != summary.end()){		
			features1withminerals.push_back(it->second*Observatory); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "FleetBeacon"))) != summary.end()){		
			features1withminerals.push_back(it->second*FleetBeacon); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Forge"))) != summary.end()){				
			features1withminerals.push_back(it->second*Forge); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "PhotonCannon"))) != summary.end()){		
			features1withminerals.push_back(it->second*Photoncannon); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Pylon"))) != summary.end()){				
			features1withminerals.push_back(it->second*Pylon); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "RoboticsFacility"))) != summary.end()){	
			features1withminerals.push_back(it->second*RoboticsFacility); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "RoboticsSupportBay"))) != summary.end()){
			features1withminerals.push_back(it->second*RoboticsSupportbay); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "ShieldBattery"))) != summary.end()){		
			features1withminerals.push_back(it->second*Shieldbattery); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "Stargate"))) != summary.end()){			
			features1withminerals.push_back(it->second*Stargate); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}
		if((it = summary.find(make_pair(1, "TemplarArchives"))) != summary.end()){	
			features1withminerals.push_back(it->second*TemplarArchives); features1.push_back(it->second);}
		else {features1withminerals.push_back(0); features1.push_back(0);}

		featurelistMINERALS.push_back(features1withminerals);
		featurelist.push_back(features1);
		for(unsigned int i =0; i< features1.size(); i++)
			cout<< features1[i] <<" ";
		cout << endl;
	}

	if(P0isProtoss){
		if((it = summary.find(make_pair(0, "Arbiter"))) != summary.end()){			
			features0withminerals.push_back(it->second*Arbiter); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Archon"))) != summary.end()){			
			features0withminerals.push_back(it->second*Archon); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "ArbiterTribunal"))) != summary.end()){	
			features0withminerals.push_back(it->second*ArbiterTribunal); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Assimilator"))) != summary.end()){		
			features0withminerals.push_back(it->second*Assimilator); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Carrier"))) != summary.end()){			
			features0withminerals.push_back(it->second*Carrier); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Corsair"))) != summary.end()){			
			features0withminerals.push_back(it->second*Corsair); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "CitadelofAdun"))) != summary.end()){		
			features0withminerals.push_back(it->second*CitadelofAdun); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "CyberneticsCore"))) != summary.end()){	
			features0withminerals.push_back(it->second*CyberneticsCore); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "DarkArchon"))) != summary.end()){		
			features0withminerals.push_back(it->second*Darkarchon); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "DarkTemplar"))) != summary.end()){		
			features0withminerals.push_back(it->second*DarkTemplar); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Dragoon"))) != summary.end()){			
			features0withminerals.push_back(it->second*Dragoon); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "HighTemplar"))) != summary.end()){		
			features0withminerals.push_back(it->second*HighTemplar); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Interceptor"))) != summary.end()){		
			features0withminerals.push_back(it->second*Interceptor); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Observer"))) != summary.end()){			
			features0withminerals.push_back(it->second*Observer); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Probe"))) != summary.end()){				
			features0withminerals.push_back(it->second*Probe); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Reaver"))) != summary.end()){			
			features0withminerals.push_back(it->second*Reaver); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Scout"))) != summary.end()){				
			features0withminerals.push_back(it->second*Scout); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Shuttle"))) != summary.end()){			
			features0withminerals.push_back(it->second*Shuttle); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Zealot"))) != summary.end()){			
			features0withminerals.push_back(it->second*Zealot); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Gateway"))) != summary.end()){			
			features0withminerals.push_back(it->second*Gateway); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Nexus"))) != summary.end()){				
			features0withminerals.push_back(it->second*Nexus); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Observatory"))) != summary.end()){		
			features0withminerals.push_back(it->second*Observatory); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "FleetBeacon"))) != summary.end()){		
			features0withminerals.push_back(it->second*FleetBeacon); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Forge"))) != summary.end()){				
			features0withminerals.push_back(it->second*Forge); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "PhotonCannon"))) != summary.end()){		
			features0withminerals.push_back(it->second*Photoncannon); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Pylon"))) != summary.end()){				
			features0withminerals.push_back(it->second*Pylon); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "RoboticsFacility"))) != summary.end()){	
			features0withminerals.push_back(it->second*RoboticsFacility); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "RoboticsSupportBay"))) != summary.end()){
			features0withminerals.push_back(it->second*RoboticsSupportbay); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "ShieldBattery"))) != summary.end()){		
			features0withminerals.push_back(it->second*Shieldbattery); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "Stargate"))) != summary.end()){			
			features0withminerals.push_back(it->second*Stargate); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}
		if((it = summary.find(make_pair(0, "TemplarArchives"))) != summary.end()){	
			features0withminerals.push_back(it->second*TemplarArchives); features0.push_back(it->second);}
		else {features0withminerals.push_back(0); features0.push_back(0);}

		featurelistMINERALS.push_back(features0withminerals);
		featurelist.push_back(features0);
		for(unsigned int i =0; i< features0.size(); i++)
			cout<< features0[i] << " ";
		cout << endl;
	}
}




