#pragma once


#include "kmeans.h"

using namespace std;

//centroids number?? 

vector<int> unitsmade;
vector<int> unitsdestroyed;
int main()
{
	
	ifstream file;
	string filename = "C:\\replays\\1.rep.rgd"; 
	file.open( filename, ifstream::out);
	char line[500];//lol limits ...
	string damncstrings;
	for(int i = 2; !file.fail(); i++){
		while(!file.eof())
		{
			file.getline(line, 1000);
			damncstrings = line;
			cout <<  damncstrings << endl;
		}

		file.clear();
		/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		ATTENTION I ASSUME YOU WILL 
		RENAME THE RGD FILES INTO 1.rep.rgd, 2.rep.rgd ... and so on
		this can be done easily with a bulk renamer.
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		filename[11] = i;
		file.open(filename , ifstream::out);
	} 

	

}