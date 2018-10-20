#include <Eigen/Sparse>
#include "HeatDiffusion/Laplacian.h"
#include <arlsmat.h>
#include <arlgsym.h>
#include "nanoflann.hpp"

using namespace nanoflann;

typedef Eigen::SparseMatrix<float> SpMat;
typedef Eigen::Triplet<float> T;

template <typename T>
struct PointCloud
{
    struct Point
    {
        T x,y,z;
    };
    std::vector<Point> pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
    inline T kdtree_distance(const T *p1, const size_t idx_p2,size_t size) const
    {
        const T d0=p1[0]-pts[idx_p2].x;
        const T d1=p1[1]-pts[idx_p2].y;
        const T d2=p1[2]-pts[idx_p2].z;
        return d0*d0+d1*d1+d2*d2;
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    // "if/else's" are actually solved at compile time.
    inline T kdtree_get_pt(const size_t idx, int dim) const
    {
        if (dim==0) return pts[idx].x;
        else if (dim==1) return pts[idx].y;
        else return pts[idx].z;
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    // Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    // Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX &bb) const { return false; }
};

namespace HeatDiffusion{




float Laplacian::determinant(Vector a, Vector b){
    return a.x()*(b.y() - b.z()) - a.y()*(b.x() - b.z()) + a.z()*(b.x() - b.y());
}

Laplacian::Laplacian()
{
    type = BELKIN;
    num_eigenvalues = 300;

    evals = NULL;
    evecs = NULL;
    areas = NULL;
}

Laplacian::~Laplacian(){
    if(evals)
        delete[] evals;
    if(evecs)
        delete[] evecs;
    if(areas)
        delete[] areas;
}

void Laplacian::setGeometryData(int numVertex, float* vertexBuffer, int numTriangles, unsigned int* indexBuffer){
    /*num_vertex = numVertex;
    //vertex_buffer = vertexBuffer;
    num_triangles = numTriangles;
    //index_buffer = indexBuffer;

    object.set_number_vertices(numVertex);
    object.set_number_triangles(numTriangles);

    for(unsigned int i = 0; i < num_vertex; i++){
        unsigned int start = 3 * i;
        object.add_vertex(i, vertexBuffer[start], vertexBuffer[start + 1], vertexBuffer[start + 2]);
    }

    for(unsigned int i = 0; i < num_triangles; i++){
        unsigned int start = 3 * i;
        object.add_triangle(i, indexBuffer[start], indexBuffer[start + 1], indexBuffer[start + 2]);
    }


    SimpleMesh::Vertex* vertices = object.get_vertices();
    SimpleMesh::Triangle* triangles = object.get_triangles();

    /*cout << object.get_number_vertices() << " - " << object.get_number_triangles() << endl;
    for(int i = 0; i < num_vertex; i++)
        cout << vertices[i].x() << ", " << vertices[i].y() << ", "<< vertices[i].z() << endl;

    for(int i = 0; i < num_triangles; i++){
        vector<unsigned int> verts = triangles[i].get_vertices();
        cout << verts[0] << ", " << verts[1] << ", "<< verts[2] << endl;
    }*/
}

void Laplacian::setObject(SimpleMesh::Mesh *input){
    object = input;
    num_vertex = object->get_number_vertices();
    num_triangles = object->get_number_triangles();
}

void Laplacian::compute(){
    if(type == BELKIN){
        computeBelkinLaplacian();
    }
    else if(type == COTAN){
        computeCotangentLaplacian();
    }

    cout << "Finish Laplacian" << endl;
    computeEigendecomposition();
}

void Laplacian::computeBelkinLaplacian(){

    AAV.clear();
    AAV.resize(num_vertex, 0);

    float aves = 0.0;

    SimpleMesh::Vertex* vertices = object->get_vertices();
    SimpleMesh::Triangle* triangles = object->get_triangles();

    //Compute the area per vertex
    for(unsigned int i = 0; i < num_triangles; i++){
        vector<unsigned int> verts = triangles[i].get_vertices();

        //unsigned int start = 3 * i;

        //unsigned int vert0 = index_buffer[start];
        //unsigned int vert1 = index_buffer[start+1];
        //unsigned int vert2 = index_buffer[start+2];

        //unsigned int start_v0 = 3*vert0;
        //unsigned int start_v1 = 3*vert1;
        //unsigned int start_v2 = 3*vert2;

        //Vector XV(vertex_buffer[start_v0], vertex_buffer[start_v1], vertex_buffer[start_v2]);
        //Vector YV(vertex_buffer[start_v0+1], vertex_buffer[start_v1+1], vertex_buffer[start_v2+1]);
        //Vector ZV(vertex_buffer[start_v0+2], vertex_buffer[start_v1+2], vertex_buffer[start_v2+2]);
        Vector XV(vertices[verts[0]].x(), vertices[verts[1]].x(), vertices[verts[2]].x());
        Vector YV(vertices[verts[0]].y(), vertices[verts[1]].y(), vertices[verts[2]].y());
        Vector ZV(vertices[verts[0]].z(), vertices[verts[1]].z(), vertices[verts[2]].z());

        float det1 = determinant(XV, YV);
        float det2 = determinant(YV, ZV);
        float det3 = determinant(ZV, XV);

        float area = 0.5*sqrt(det1*det1 + det2*det2 + det3*det3);

        AAV[verts[0]] += area/3;
        AAV[verts[1]] += area/3;
        AAV[verts[2]] += area/3;

        //Vector v1(vertex_buffer[start_v0] - vertex_buffer[start_v1], vertex_buffer[start_v0+1] - vertex_buffer[start_v1+1], vertex_buffer[start_v0+2] - vertex_buffer[start_v1+2]);
        //Vector v2(vertex_buffer[start_v1] - vertex_buffer[start_v2], vertex_buffer[start_v1+1] - vertex_buffer[start_v2+1], vertex_buffer[start_v1+2] - vertex_buffer[start_v2+2]);
        //Vector v3(vertex_buffer[start_v2] - vertex_buffer[start_v0], vertex_buffer[start_v2+1] - vertex_buffer[start_v0+1], vertex_buffer[start_v2+2] - vertex_buffer[start_v0+2]);
        Vector v1(vertices[verts[0]].x() - vertices[verts[1]].x(), vertices[verts[0]].y() - vertices[verts[1]].y(), vertices[verts[0]].z() - vertices[verts[1]].z());
        Vector v2(vertices[verts[1]].x() - vertices[verts[2]].x(), vertices[verts[1]].y() - vertices[verts[2]].y(), vertices[verts[1]].z() - vertices[verts[2]].z());
        Vector v3(vertices[verts[2]].x() - vertices[verts[0]].x(), vertices[verts[2]].y() - vertices[verts[0]].y(), vertices[verts[2]].z() - vertices[verts[0]].z());

        aves += sqrt(fabs(v1.dot(v1)));
        aves += sqrt(fabs(v2.dot(v2)));
        aves += sqrt(fabs(v3.dot(v3)));
    }

    aves /= 3*num_triangles;

    //Compute the laplacian matrix
    vector<float> totalweight;
    totalweight.resize(num_vertex, 0);

    float h = 2 * aves;
    //cout << "h=" << h << endl;
    float rho = 3;

    float hh = h * h;

    //Build the index
    PointCloud<float> points;
    points.pts.resize(num_vertex);
    for(unsigned int i = 0; i < num_vertex; i++){
        //points.pts[i].x = vertex_buffer[3 * i];
        //points.pts[i].y = vertex_buffer[3 * i + 1];
        //points.pts[i].z = vertex_buffer[3 * i + 2];
        points.pts[i].x = vertices[i].x();
        points.pts[i].y = vertices[i].y();
        points.pts[i].z = vertices[i].z();
    }

    typedef KDTreeSingleIndexAdaptor< L2_Simple_Adaptor<float, PointCloud<float> >, PointCloud<float>, 3> my_kdtree;
    my_kdtree index(3, points, KDTreeSingleIndexAdaptorParams(10));
    index.buildIndex();

    for(unsigned int i = 0; i < num_vertex; i ++){
        const float search_radius = static_cast<float>(h * rho);
        std::vector<std::pair<size_t,float> > ret_matches;
        nanoflann::SearchParams params;

        //const float query[3]={vertex_buffer[3*i], vertex_buffer[3*i + 1], vertex_buffer[3*i + 2]};
        const float query[3]={vertices[i].x(), vertices[i].y(), vertices[i].z()};

        const size_t nMatches = index.radiusSearch(&query[0], search_radius*search_radius, ret_matches, params);

        for(unsigned int j = 0; j < ret_matches.size(); j++){
            unsigned int vid = ret_matches[j].first;
            if(vid <= i){
                continue;
            }

            float weight = exp(-ret_matches[j].second / hh) * ( 4.0 / (M_PI * hh * hh) );

            weight *=  AAV[vid] * AAV[i];

            IIV.push_back(i);
            JJV.push_back(vid);
            SSV.push_back(weight);

            IIV.push_back(vid);
            JJV.push_back(i);
            SSV.push_back(weight);

            totalweight[i] -= weight;
            totalweight[vid] -= weight;
        }
    }

    for(unsigned int i = 0; i < num_vertex; i ++){
        IIV.push_back(i);
        JJV.push_back(i);
        SSV.push_back(totalweight[i]);
    }

    cout << IIV.size() << " - " << JJV.size() << " - " << SSV.size() << " - " << AAV.size() << endl;
}

void Laplacian::computeCotangentLaplacian(){

//    vector<float> fareas;
//    fareas.resize(3 * num_triangles);

//    vector<float> fcots;
//    fcots.resize(3 * num_triangles);

//    unsigned int vert0, vert1, vert2;
//    SimpleMesh::Vertex* vertices = object->get_vertices();
//    SimpleMesh::Triangle* triangles = object->get_triangles();

//    for(unsigned int i = 0; i < num_triangles; i++){
//        //unsigned int start = 3 * i;

//        //vert0 = index_buffer[start];
//        //vert1 = index_buffer[start + 1];
//        //vert2 = index_buffer[start + 2];

//        //unsigned int start_v0 = 3*vert0;
//        //unsigned int start_v1 = 3*vert1;
//        //unsigned int start_v2 = 3*vert2;

//        vector<unsigned int> verts = triangles[i].get_vertices();

//        //Vector XV(vertex_buffer[start_v0], vertex_buffer[start_v1], vertex_buffer[start_v2]);
//        //Vector YV(vertex_buffer[start_v0+1], vertex_buffer[start_v1+1], vertex_buffer[start_v2+1]);
//        //Vector ZV(vertex_buffer[start_v0+2], vertex_buffer[start_v1+2], vertex_buffer[start_v2+2]);

//        Vector XV(vertices[verts[0]].x(), vertices[verts[1]].x(), vertices[verts[2]].x());
//        Vector YV(vertices[verts[0]].y(), vertices[verts[1]].y(), vertices[verts[2]].y());
//        Vector ZV(vertices[verts[0]].z(), vertices[verts[1]].z(), vertices[verts[2]].z());

//        float det1 = determinant(XV, YV);
//        float det2 = determinant(YV, ZV);
//        float det3 = determinant(ZV, XV);

//        float area = 0.5*sqrt(det1*det1 + det2*det2 + det3*det3);

//        //Measure edge lengths for triangle
//        Vector v0(vertices[verts[0]].x(), vertices[verts[0]].y(), vertices[verts[0]].z());
//        Vector v1(vertices[verts[1]].x(), vertices[verts[1]].y(), vertices[verts[1]].z());
//        Vector v2(vertices[verts[2]].x(), vertices[verts[2]].y(), vertices[verts[2]].z());

//        Vector v12(v1.x()-v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
//        Vector v02(v0.x()-v2.x(), v0.y() - v2.y(), v0.z() - v2.z());
//        Vector v10(v1.x()-v0.x(), v1.y() - v0.y(), v1.z() - v0.z());

//        float l0 = sqrt(fabs(v12.dot(v12)));
//        float l1 = sqrt(fabs(v02.dot(v02)));
//        float l2 = sqrt(fabs(v10.dot(v10)));

//        assert(l0 > 1e-10 && l1 > 1e-10 && l2 > 1e-10);

//        float h0 = area / l0;
//        float h1 = area / l1;
//        float h2 = area / l2;

//        fcots[3 * i] = sqrt(fabs(l1 * l1 - h2 * h2)) / h2;
//        if((l1 * l1 + l2 * l2 - l0 * l0 ) < 0)
//            fcots[3 * i] = -fcots[3 * i];

//        fcots[3 * i + 1] = sqrt(fabs(l2 * l2 - h0 * h0)) / h0;
//        if((l0 * l0 + l2 * l2 - l1 * l1) < 0)
//            fcots[3 * i + 1] = -fcots[3 * i + 1];

//        fcots[3 * i + 2] = sqrt(fabs(l0 * l0 - h1 * h1)) / h1;
//        if((l0 * l0 + l1 * l1 - l2 * l2) < 0)
//            fcots[3 * i + 2] = -fcots[3 * i + 2];

//        if(fcots[3 * i] >= 0 && fcots[3 * i + 1] >= 0 && fcots[3 * i + 2] >=0){
//            fareas[3 * i] = 1.0 / 8.0 * (l1 * l1 * fcots[3 * i + 1] + l2 * l2 * fcots[3 * i + 2]);
//            fareas[3 * i + 1] = 1.0 / 8.0 * (l0 * l0 * fcots[3 * i] + l2 * l2 * fcots[3 * i + 2]);
//            fareas[3 * i + 2] = 1.0 / 8.0 * (l1 * l1 * fcots[3 * i + 1] + l0 * l0 * fcots[3 * i]);
//        }else if(fcots[3 * i] < 0){
//            fareas[3 * i] = area / 2;
//            fareas[3 * i + 1] = area / 4;
//            fareas[3 * i + 2] = area / 4;
//        }else if(fcots[3 * i + 1] < 0){
//            fareas[3 * i] = area / 4;
//            fareas[3 * i + 1] = area / 2;
//            fareas[3 * i + 2] = area / 4;
//        }else{
//            fareas[3 * i] = area / 4;
//            fareas[3 * i + 1] = area / 4;
//            fareas[3 * i + 2] = area / 2;
//        }

//    }

//    AAV.clear();
//    AAV.resize(num_vertex, 0);

//    float temp, totalweight;
//    map<unsigned int, float> adj_weight;

//    for(unsigned int i = 0; i < num_vertex; i++){
//        temp = 0;
//        totalweight = 0;
//        adj_weight.clear();

//        set<unsigned int> neighbors = vertices[i].get_vertices();
//        for(set<unsigned int>::iterator it = neighbors.begin(); it!=neighbors.end(); ++it){
//            adj_weight[*it] = 0;
//        }

//        vector<unsigned int> neighFaces = vertices[i].get_triangles();
//        for(vector<unsigned int>::iterator it = neighFaces.begin(); it != neighFaces.end(); ++it){
//            unsigned int f = *it;
//            unsigned int ind = triangles[f].index(i);

//            assert(ind >= 0);

//            vector<unsigned int> verts = triangles[f].get_vertices();

//            temp += fareas[3 * f + ind];

//            unsigned int vid1 = verts[(ind + 1) % 3];
//            unsigned int vid2 = verts[(ind + 2) % 3];

//            map<unsigned int, float>::iterator iter;
//            iter = adj_weight.find(vid1);
//            assert(iter != adj_weight.end());
//            iter->second = iter->second + fcots[3 * f  + (ind + 2) % 3];

//            iter = adj_weight.find(vid2);
//            assert(iter != adj_weight.end());
//            iter->second = iter->second + fcots[3 * f + (ind + 1) % 3];

//            totalweight -= (fcots[3 * f + (ind + 2)%3] + fcots[3 * f + (ind + 1)%3]);

//        }

//        assert(temp > 0);
//        temp *= 2;

//        for(map<unsigned int, float>::iterator iter = adj_weight.begin(); iter != adj_weight.end(); iter++){
//            IIV.push_back(i);
//            JJV.push_back(iter->first);
//            SSV.push_back(iter->second);
//        }

//        IIV.push_back(i);
//        JJV.push_back(i);
//        SSV.push_back(totalweight);

//        AAV[i] = temp;
//    }
}

void Laplacian::computeEigendecomposition(){

    vector<unsigned int> _IIV;
    vector<unsigned int> _JJV;
    vector<float> _SSV;

    for(unsigned int i = 0; i < IIV.size(); i++){
        if(IIV[i] <= JJV[i]){
            _IIV.push_back(IIV[i]);
            _JJV.push_back(JJV[i]);
            _SSV.push_back(SSV[i]);
        }
    }

    IIV.clear();
    JJV.clear();
    SSV.clear();

    vector<T> tripletsLaplacian;

    for(unsigned int i =0; i < _IIV.size(); i++){
        tripletsLaplacian.push_back(T(_IIV[i], _JJV[i], _SSV[i]));
    }

    SpMat laplacian(num_vertex, num_vertex);
    laplacian.setFromTriplets(tripletsLaplacian.begin(), tripletsLaplacian.end());
    laplacian.makeCompressed();

    float* valuesLaplacian = laplacian.valuePtr();
    int* irowLaplacian =  laplacian.innerIndexPtr();
    int* _pcolLaplacian = laplacian.outerIndexPtr();

    int* pcolLaplacian = new int[laplacian.outerSize() + 1];
    for(unsigned int i = 0; i < laplacian.outerSize(); i++)
        pcolLaplacian[i] = _pcolLaplacian[i];
    pcolLaplacian[laplacian.outerSize()] = num_vertex;


    vector<T> tripletsArea;
    for(unsigned int i = 0; i < AAV.size(); i++)
        tripletsArea.push_back(T(i, i, AAV[i]));

    SpMat area(num_vertex,num_vertex);
    area.setFromTriplets(tripletsArea.begin(), tripletsArea.end());
    area.makeCompressed();

    float* valuesArea = area.valuePtr();
    int* irowArea = area.innerIndexPtr();
    int* _pcolArea = area.outerIndexPtr();

    int* pcolArea = new int[area.outerSize() + 1];
    for(unsigned int i = 0; i < area.outerSize(); i++)
        pcolArea[i] = _pcolArea[i];
    pcolArea[area.outerSize()] = num_vertex;

    unsigned int n = num_vertex;
    unsigned int nnzA = _IIV.size();
    unsigned int nnzB = num_vertex;

    ARluSymMatrix<float> A(n, nnzA, valuesLaplacian, irowLaplacian, pcolLaplacian, 'U');
    ARluSymMatrix<float> B(n, nnzB, valuesArea, irowArea, pcolArea, 'U');

    ARluSymGenEig<float> dprob(num_eigenvalues, A, B, "SM");

    evals = new float[num_eigenvalues];
    evecs = new float[n*num_eigenvalues];

    cout << "Aqui" << endl;
    dprob.EigenValVectors(evecs, evals);

    areas = new float[n];
    for(unsigned int i = 0; i < n; i++)
        areas[i] = AAV[i];

    //Assuming num_eigenvalues is always even
    //Transform the output buffers to the correct form
    for(unsigned int i = 0; i < num_eigenvalues/2; i++){
        float aux = evals[i];
        evals[i] = evals[num_eigenvalues - i - 1];
        evals[num_eigenvalues - i - 1] = aux;
    }

    for(unsigned int i = 0; i < num_eigenvalues/2; i++){
        unsigned int block_position_1 = n * i;
        unsigned int block_position_2 = n * (num_eigenvalues - i - 1);
        for(unsigned int j = 0; j < n; j++){
            float aux = evecs[block_position_1 + j];
            evecs[block_position_1 + j] = evecs[block_position_2 + j];
            evecs[block_position_2 + j] = aux;
        }
    }
}

}//namespace Prepro
