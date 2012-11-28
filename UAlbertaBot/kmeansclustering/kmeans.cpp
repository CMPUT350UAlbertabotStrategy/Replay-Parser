#pragma once


#include "kmeans.h"

using namespace std;

//centroids number?? 

vector<int> unitsmade;
vector<int> unitsdestroyed;
int main()
{
	//disclaimer! crazy hacks.

	ifstream file;
	string filename = "C:\\replays\\1.rep.rgd"; 
	file.open( filename, ifstream::out);
	char line[500];//lol limits ...
	string damncstrings;
	for(unsigned int i = 2; !file.fail(); i++){
		while(!file.eof())
		{
			file.getline(line, 1000);
			damncstrings = line;
			cout <<  damncstrings << endl;
		}
		file.close();
		file.clear();//so I can reuse the ifstream
		/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		ATTENTION I ASSUME YOU WILL 
		RENAME THE RGD FILES INTO 1.rep.rgd, 2.rep.rgd ... and so on
		this can be done easily with a bulk renamer.
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		filename[11] = boost::lexical_cast<char>(i); 
		//YES you need a lexical cast.
		//recall this "C:\\replays\\X.rep.rgd" from above i am modifiying
		// the position x with an integer
		file.open(filename , ifstream::out);
	} 

	

}