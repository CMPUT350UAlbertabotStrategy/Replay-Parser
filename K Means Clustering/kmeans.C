/*
 * kmeans.C
 *
 *  Created on: Nov 28, 2012
 *      Author: ba6



using namespace std;



class Strategy
{
public:
	Strategy()
	{
		// freakout, should not be allowed
	}

	Strategy(vector<int> units){
		// TODO: convert the units seen into their resource costs
		features = units;
	}
	int cluster;
	vector<int> features;
};

// get similarity is driven by an attempt at using dot product as an approximation for similarity between strategies.
// I have no guarantees that this is a sensible or appropriate way to evaluate similarity.
// Conceptually, similarity between strategies is based on resources sunk into following that strategy, or the opportunity cost missed
// in following that strategy (in the case of low economy rushes etc). Thus, we attempt to treat the resources spent in buildings and units
// as vectors and compare them.
// returns 0 if the strategies are identical, smaller return is greater similarity
double getSimilarity(Strategy &st1, Strategy &st2)
{
	long int dot_product = 0;
	long int dot_identity = 0;

	// calculate the dot product of st1 . st2, and st2 with itself
	for (int i = 0; i < st2.features.size(); i++)
	{
		dot_product += st1.features[i] * st2.features[i];
		dot_identity += st2.features[i] * st2.features[i];
	}

	// divide the dot product by the 'dot identity' in an attempt to norm for resouces gather
	// a low econ strategy should look different than a high econ strategy
	// subtract 1 and take the absolute value so we get how close it is to 1
	if (dot_identity == 0)
		return -1;
	return fabs((dot_product/dot_identity)-1);

}

class Cluster
{
public:
	vector<Strategy*> members;
	Strategy centroid;
	string strategy_label;

	Cluster()
	{
		// freak out, should not be allowed!
	}

	Cluster(vector<int> units)
	{
		members.push_back(new Strategy(units));
		centroid = calculateCentroid();
	}

	// creates the center of the cluster based on a simple average for each feature
	Strategy calculateCentroid()
	{
		Strategy old_centroid = centroid;
		// TODO: reset the centroid
		for (int i = 0; i < members.size(); i++)
			for (int j = 0; j < members[i]->features.size(); j++)
				centroid.features[j] += members[i]->features[j];

		for (int k = 0; k < centroid.features.size(); k++)
			centroid.features[k] = centroid.features[k] / members.size();

		return centroid;

	}

	double update(vector<Strategy*> new_members){
		Strategy old_centroid = centroid;
		members = new_members;
		centroid = calculateCentroid();
		return getSimilarity(old_centroid, centroid);
	}
};



vector<Cluster> kmeans(vector<vector<int> >Strategy_list, int k, double cutoff)
{
	vector<Cluster> clusters;
	for (int i = 0; i < k; i++)
	{
		clusters.push_back(new Cluster(Strategy_list[i]));
	}

	while (true)
	{
		vector<vector<int> > cluster_list;
		// for each strategy
		for (int i = 0; i < Strategy_list.size(); i++)
		{
			// find the cluster with the most similar centoid
			double most_similar = getSimilarity(Strategy_list[i], clusters[0].centroid);
			int best_index = 0;
			for (int j = 1; j < clusters.size(); j++)
			{
				// if the jth cluster is more similar than any seen so far, keep track
				double similarity = getSimilarity(Strategy_list[i], clusters[j].centroid);
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

		for (int i = 0; i < clusters.size(); i++)
		{
			double shift = clusters[i].update(cluster_list(i));
			if (shift > biggest_shift)
			{
				biggest_shift = shift;
			}
		}
		// clustering will loop until shifts have dropped below the cutoff level
		if (biggest_shift < cutoff)
			break;
	}

	// TODO: return centroids, not cluster
	return clusters;

}

*/