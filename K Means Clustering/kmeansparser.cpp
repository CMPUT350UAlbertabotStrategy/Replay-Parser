#include "kmeans.h"

using namespace std;

//centroids number?? 


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
	FILE * input= _popen("dir *.rep.rgd /b", "r");
	ifstream thefile("1.rep.rgd");
	//this line is messed up beyond belief
	//this line will not work without a literal in quotes. So now it is nessessary to have a start
	//file called 1.rep.rgd, but otherwise this will work with files ending in .rep.rgd
	while(!feof(input))
	{
		fgets(file, 199, input);
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
				//something to do with vecotr unitsdestroyed
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
	_pclose(input);
	//==================================
}
