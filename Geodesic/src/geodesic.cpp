#include <QtGlobal>
#include "Geodesic/geodesic.h"


namespace Geodesic{


GeodesicEngine::GeodesicEngine()
{
    input = NULL;
}

void GeodesicEngine::setData(SimpleMesh::Mesh *data){
    SimpleMesh::Vertex* vertices = data->get_vertices();
    SimpleMesh::Triangle* triangles = data->get_triangles();

    gm.SetNbrVertex(data->get_number_vertices());

    for(int i = 0; i < data->get_number_vertices(); i++){
        GW_GeodesicVertex& vert = (GW_GeodesicVertex&) gm.CreateNewVertex();
        vert.SetPosition( GW_Vector3D(vertices[i].x(), vertices[i].y(), vertices[i].z()) );
        gm.SetVertex(i, &vert);
    }

    gm.SetNbrFace(data->get_number_triangles());
    for(int i = 0; i < data->get_number_triangles(); i++){
        vector<unsigned int> vert = triangles[i].get_vertices();
        GW_GeodesicFace& face = (GW_GeodesicFace&) gm.CreateNewFace();
        GW_Vertex* v1 = gm.GetVertex((int) vert[0]); GW_ASSERT( v1 != NULL);
        GW_Vertex* v2 = gm.GetVertex((int) vert[1]); GW_ASSERT( v2 != NULL);
        GW_Vertex* v3 = gm.GetVertex((int) vert[2]); GW_ASSERT( v3 != NULL);
        face.SetVertex(*v1, *v2, *v3);
        gm.SetFace(i, &face);
    }

    gm.BuildConnectivity();
}

void GeodesicEngine::computeGeodesicDistances(int indexVertex, float *distances){
    cout << "GeodesicEngine::computeGeodesicDistances(" << indexVertex << ")" << endl;
    gm.ResetGeodesicMesh();

    GW_GeodesicVertex* v = (GW_GeodesicVertex*)gm.GetVertex((GW_U32) indexVertex);
    GW_ASSERT( v != NULL );
    gm.AddStartVertex( *v );

    gm.SetUpFastMarching();

    GW_Float* weights = new GW_Float[gm.GetNbrVertex()];
    for(int i = 0; i < gm.GetNbrVertex(); i++)
        weights[i] = 1;

    GW_Float* dist = new GW_Float[gm.GetNbrVertex()];
    for(int i = 0; i < gm.GetNbrVertex(); i++)
        dist[i] = 1e9;

    gm.setWeights(weights);
    gm.setStopInformation(NULL, 0, 1e9);
    gm.setInsertionConstraint(dist, 1.2*gm.GetNbrVertex());

    gm.PerformFastMarching();

    for(int i = 0; i < gm.GetNbrVertex(); i++){
        GW_GeodesicVertex* v = (GW_GeodesicVertex*)gm.GetVertex((GW_U32) i);
        distances[i] = v->GetDistance();
        //cout << distances[i] << " ";
    }
    //cout << endl;
}


void GeodesicEngine::performFarthestPointSampling(int numPoints, vector<int>& points){
   int n = gm.GetNbrVertex();
   int point = qrand() % n;

   float* D = new float[n];
   computeGeodesicDistances(point, D);

   float max =-1;
   int pos=-1;

   for(int i = 0; i < n; i++){
        if(D[i] < 1e8){
            if(D[i] > max){
                max = D[i];
                pos = i;
            }
        }
   }

   delete[] D;
   points.push_back(pos);

   float* DD = new float[n];
   computeGeodesicDistances(pos, DD);

   for(int i = 1 ; i < numPoints; i++){
       max = -1;
       pos = -1;

       for(int j = 0; j < n; j++){
            if(DD[j] < 1e8){
                if(DD[j] > max){
                    max = DD[j];
                    pos = j;
                }
            }
       }

       points.push_back(pos);
       float* Daux = new float[n];
       computeGeodesicDistances(pos, Daux);

       for(int j = 0; j < n; j++)
           DD[j] = DD[j] < Daux[j]? DD[j] : Daux[j];

       delete[] Daux;
   }

   delete[] DD;

}

} //namespace
