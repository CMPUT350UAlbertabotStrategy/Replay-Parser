

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>

using namespace std;


int main()
{
	//
	vector<int> player1, player2;
	string temp;
	string line;
	ofstream file;
	file.open("zvp1.rep.rgd");


	//maybe also destroyed units in a vector??
	// 
  	regex e ("(\\d+),(\\d+),(Created|Morph),(\\d+),([ a-zA-Z]+),(\\d+,\\d+)");   // matches lines from rgd files


	if(file.is_open()){

		while(file.good()){
			line = file.getline();
			regex_match(temp, line,e);

		}


		file.close();
	}




}


vector<int> parse()
{

}