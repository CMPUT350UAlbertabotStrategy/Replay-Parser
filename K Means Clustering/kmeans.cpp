#pragma once
#include "kmeans.h"

using namespace std;

//centroids number?? 

ifstream file;
string damncstrings;
string filename = "C:\\replays\\"; 
char line[500];
int player;
int numberofunits;
string unit;
vector<int> unitsmade;
vector<int> unitsdestroyed;
int main()
{
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ATTENTION I ASSUME YOU WILL 
	RENAME THE RGD FILES INTO 1, 2, 3, 4, 5 ... and so on
	this can be done easily with a bulk renamer.
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	file.open(filename + boost::lexical_cast<char>(1));//YES you need a lexical cast.
	for(unsigned int i = 2; !file.fail(); i++){
		bool flagcreated =false , flagdestroyed =false;
		while(!file.eof())
		{
			file.getline(line, 500);
			damncstrings = line;
			if(damncstrings == "[UNITS LOST]") 
			{cout<< i <<"[UNITS LOST]"<< endl; flagdestroyed = true;}
			if(damncstrings == "[UNITS CREATED]") 
			{cout<<i <<"[UNITS CREATED]"<< endl; flagcreated = true; flagdestroyed = false;}

			if(flagdestroyed == true)
			{
				istringstream stream(damncstrings);
				stream >> player >> unit >> unit >> numberofunits;
				cout <<  boost::format("%s %s %s") %player %unit %numberofunits << endl;
				//something to do with vecotr unitsdestroyed
			}
			if(flagcreated == true)
			{
				istringstream stream(damncstrings);
				stream >> player >> unit >> unit >> numberofunits;
				cout <<  boost::format("%s %s %s") %player %unit %numberofunits << endl;
				//something to do with vector unitsmade.
			}

		}
		file.close();//close for next file
		file.clear();//so I can reuse the ifstream
		file.open(filename + boost::lexical_cast<char>(i));//YES you need a lexical cast.
	} 
}