#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <map>

//holds ALL units(and id of player) and the time they were made.
struct data{
	data(std::string unit, int frame, int player): unit(unit), frame(frame), player(player){}
	std::string unit;
	int frame;
	int player;
};
void sort(std::map<std::pair<int, std::string>, int>);
struct gamesummary{
	~gamesummary(){unitsdestroyed.clear(); unitsmade.clear();}
	std::vector<data> unitsdestroyed;
	std::vector<data> unitsmade;
};
//similar to getsummary but is contained within a time interval and values are in a map.
struct gamecountinterval{
	gamecountinterval(){}
	gamecountinterval(int low, int high): lowerboundry(low) , higherboundry(high){}
	~gamecountinterval(){unitsmade.clear(); unitsdestroyed.clear();}
	//pairs are good since they are ordered by default very well
	std::map<std::pair<int, std::string>, int> unitsdestroyed;
	std::map<std::pair<int, std::string>, int> unitsmade;
	int lowerboundry, higherboundry;
};
gamecountinterval *	summarytointerval(int lowerboundry, int higherboundry);









