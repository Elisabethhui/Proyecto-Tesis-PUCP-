#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include "adaptive_clustering_global.h"
#include <vector>
#include "PointND.h"
using namespace std;

//extern double computeDistance(AdaptiveClustering::PointND a, AdaptiveClustering::PointND b);

namespace AdaptiveClustering{

class ADAPTIVE_API Cluster{
	int dim;
	PointND centroid;
	vector< PointND > points;

	int indexCentroid;
	vector<int> members;

public:
	Cluster(){ dim = 0; }
	Cluster(int d) { dim = d; }

	~Cluster(){}

	vector<int>& getElements(){return members;}
	void computeCentroid();
    void computeMedoid(float* distances, int N);

	PointND getCentroid(){return centroid;}
	int getIndexCentroid(){return indexCentroid;}

	void addPoint(PointND p){ points.push_back(p); }
	void addIndex(int ind){ members.push_back(ind);}

	void removePoints(vector< PointND >& points);
	void removeIndex(vector<int>& marks);

	int getSize(){ return points.size();}
	int getSizeIndex() {return members.size();}

	int getIndexAt(int pos){return points[pos].index;}

	int getIndex(int pos){ return members[pos];}
	int getCloser();

    float computeDistance(PointND a, PointND b);

	};
}
#endif
