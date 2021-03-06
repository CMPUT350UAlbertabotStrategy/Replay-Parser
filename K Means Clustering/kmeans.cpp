/*
 * kmeans.C
 *
 *  Created on: Nov 28, 2012
 *      Author: ba6
 */
#include "kmeans.h"

#pragma once

using namespace std;

Strategy::Strategy()
{

}

Strategy::Strategy(std::vector<double> units){
	// TODO: convert the units seen into their resource costs
	features = units;
}


// get similarity is driven by an attempt at using dot product as an approximation for similarity between strategies.
// I have no guarantees that this is a sensible or appropriate way to evaluate similarity.
// Conceptually, similarity between strategies is based on resources sunk into following that strategy, or the opportunity cost missed
// in following that strategy (in the case of low economy rushes etc). Thus, we attempt to treat the resources spent in buildings and units
// as vectors and compare them.
// returns 0 if the strategies are identical, smaller return is greater similarity
double getSimilarity(Strategy* st1, Strategy* st2)
{
	long double dot_product = 0;
	long double dot_identity = 0;

	if (st1->features.size() != st2->features.size())
	{
		std::cout << "mismach of vector sizes between target and centroid!";
		exit(20);
	}
	// calculate the dot product of st1 . st2, and st2 with itself
	for (unsigned int i = 0; i < st2->features.size(); i++)
	{
		std::vector<double> temp = st1->features;
		std::vector<double> temp2 = st2->features;
		dot_product += st1->features[i] * st2->features[i];
		dot_identity += st2->features[i] * st2->features[i];
	}

	// divide the dot product by the 'dot identity' in an attempt to norm for resouces gather
	// a low econ strategy should look different than a high econ strategy
	// subtract 1 and take the absolute value so we get how close it is to 1
	if (dot_identity == 0)
		return -1;
	return fabs((dot_product/dot_identity)-1);

}

Cluster::Cluster(std::vector<double> units)
{
	Strategy* new_start = new Strategy(units);
	members.push_back(new_start);
	centroid = new_start;
}

// creates the center of the cluster based on a simple average for each feature
Strategy* Cluster::calculateCentroid()
{
//	Strategy old_centroid = centroid;
	// TODO: reset the centroid
	for (unsigned int i = 0; i < centroid->features.size(); i++)
		centroid->features[i] = 0;


	for (unsigned int i = 0; i < members.size(); i++)
		for (unsigned int j = 0; j < members[i]->features.size(); j++)
			centroid->features[j] += members[i]->features[j];

	int temp = members.size();
	for (unsigned int k = 0; k < centroid->features.size(); k++)
		centroid->features[k] = centroid->features[k] / members.size();

	return centroid;

}

double Cluster::update(std::vector<Strategy*> new_members){
	Strategy* old_centroid = centroid;
	members = new_members;
	centroid = calculateCentroid();
	return getSimilarity(old_centroid, centroid);
}

std::vector<Strategy*> kmeans(std::vector<std::vector<double> >unit_list, int k, double cutoff)
{
	std::vector<Cluster*> clusters;
	for (int i = 0; i < k; i++)
	{
		clusters.push_back(new Cluster(unit_list[i]));
	}

	std::vector<Strategy* > Strategy_list;
	for (unsigned int i = 0; i < unit_list.size(); i++)
	{
		Strategy_list.push_back(new Strategy(unit_list[i]));
	}

	while (true)
	{
		std::vector<std::vector<Strategy*> > cluster_list;
		for (int i = 0; i < k; i++)
		{
			std::vector<Strategy*>* new_strat_vector = new std::vector<Strategy*>;
			cluster_list.push_back(*new_strat_vector);
		}

		// for each strategy
		for (unsigned int i = 0; i < Strategy_list.size(); i++)
		{
			// find the cluster with the most similar centoid
			double most_similar = getSimilarity(Strategy_list[i], clusters[0]->centroid);
			int best_index = 0;
			for (unsigned int j = 1; j < clusters.size(); j++)
			{
				// if the jth cluster is more similar than any seen so far, keep track
				double similarity = getSimilarity(Strategy_list[i], clusters[j]->centroid);
				if (similarity < most_similar)
				{
					most_similar = similarity;
					best_index = j;
				}
			}

			// add the strategy to the most similar cluster_list
			cluster_list[best_index].push_back(Strategy_list[i]);
		}

		double biggest_shift = 0;

		for (unsigned int i = 0; i < clusters.size(); i++)
		{
			double shift = clusters[i]->update(cluster_list[i]);
			if (shift > biggest_shift)
			{
				biggest_shift = shift;
			}
		}
		// clustering will loop until shifts have dropped below the cutoff level
		if (biggest_shift < cutoff)
			break;
	}

	std::vector<Strategy*> centroids;
	for (unsigned int i = 0; i < clusters.size(); i++)
		centroids.push_back(clusters[i]->centroid);

	return centroids;

}
