#include "kmeans.h"

using namespace std;

string theline;
char file[200];
int player;
int numberofunits;
string unit;
vector<int> unitsdestroyed;

int main()
{
	//this program requires itself to be located in the same folder as the rep.rgd files
	//==================================
	FILE * pipe= _popen("dir *.rep.rgd /b", "r");//use windows command dir with argument /b for simple file listing add /s for full path
	ifstream thefile(""); //empty string to get the first item
	//ifstream should default construct with emtpy string... no single help place 
	while(!feof(pipe))
	{
		fgets(file, 199, pipe);
		cout << file;
		
		bool flagcreated =false , flagdestroyed =false;
		while(getline(thefile, theline))
		{
			if(theline == "[UNITS LOST]"){ flagdestroyed = true;}
			if(theline == "[UNITS CREATED]"){ flagcreated = true; flagdestroyed = false;}

			if(flagdestroyed == true)
			{
				istringstream stream(theline);
				stream >> player >> unit >> unit >> numberofunits;
				//cout <<  player<< " " << unit<< " " <<" " <<numberofunits;
				//something to do with vector unitsdestroyed
			}
			if(flagcreated == true)
			{
				istringstream stream(theline);
				stream >> player >> unit >> unit >> numberofunits;
				//cout <<  player<< " " << unit<< " " <<" " <<numberofunits;
				//something to do with vector unitsmade.
			}
		}
		thefile.close();
		thefile.open(file);
	} 
	_pclose(pipe);
	//==================================
}
