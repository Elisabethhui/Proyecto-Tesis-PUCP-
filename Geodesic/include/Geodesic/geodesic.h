#ifndef GEODESIC_H
#define GEODESIC_H

#include "Geodesic/geodesic_global.h"
#include "SimpleMesh/mesh.h"
#include "Geodesic/gw_geodesic/GW_GeodesicMesh.h"

namespace Geodesic{

class GEODESIC_API GeodesicEngine{
private:
    SimpleMesh::Mesh* input;
    GW_GeodesicMesh gm;

public:
    GeodesicEngine();
    void setData(SimpleMesh::Mesh* data);

    //Functions to compute information

    // It computes the geodesic distances between a vertex and the remaining ones. It returns the distances by
    // parameter. The array "distances" should be allocated before calling this function
    void computeGeodesicDistances(int indexVertex, float* distances);

    //It computes a farthest point sampling for a given mesh. It returns the indices of sampled points and the
    //distances to vertices in the mesh
    void performFarthestPointSampling(int numPoints, vector<int>& points);
};
}

#endif // GEODESIC_H
