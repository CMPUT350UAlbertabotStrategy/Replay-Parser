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

struct gamesummary{
	~gamesummary(){unitsdestroyed.clear(); unitsmade.clear();}
	std::vector<data*> unitsdestroyed;
	std::vector<data*> unitsmade;
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
void protossfeatureV(std::map<std::pair<int, std::string>, int>);

class Strategy
{
public:
	Strategy();
	Strategy(std::vector<double> units);
	int cluster;
	std::vector<double> features;
};

// get similarity is driven by an attempt at using dot product as an approximation for similarity between strategies.
// I have no guarantees that this is a sensible or appropriate way to evaluate similarity.
// Conceptually, similarity between strategies is based on resources sunk into following that strategy, or the opportunity cost missed
// in following that strategy (in the case of low economy rushes etc). Thus, we attempt to treat the resources spent in buildings and units
// as vectors and compare them.
// returns 0 if the strategies are identical, smaller return is greater similarity
double getSimilarity(Strategy* st1, Strategy* st2);

class Cluster
{
public:
	std::vector<Strategy*> members;
	Strategy* centroid;

	Cluster();
	Cluster(std::vector<double> units);

	// creates the center of the cluster based on a simple average for each feature
	Strategy* calculateCentroid();

	double update(std::vector<Strategy*> new_members);
};

std::vector<Strategy*> kmeans(std::vector<std::vector<double> >unit_list, int k, double cutoff);







