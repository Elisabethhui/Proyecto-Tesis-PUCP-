#ifndef __ADAPTIVECLUSTERING_H_
#define __ADAPTIVECLUSTERING_H_

#include "adaptive_clustering_global.h"
#include "PointND.h"
#include "cluster.h"

namespace AdaptiveClustering{

#ifdef _cplusplus
extern "C"{
#endif // _cplusplus

//ADAPTIVE_API double computeDistance(PointND a, PointND b);
void ADAPTIVE_API adaptiveClusteringMedoids(float* distances, int numPoints, float R, float S, int Nm, int Iter, int option, vector<Cluster>& clusters);
void ADAPTIVE_API adaptiveClustering(vector<PointND>& points, double R, double S, double Nm, int Iter,int option,vector<Cluster>& clusters );

#ifdef _cplusplus
}
#endif // _cplusplus
}
#endif
