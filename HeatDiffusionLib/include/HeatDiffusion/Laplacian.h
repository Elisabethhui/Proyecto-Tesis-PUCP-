#ifndef LAPLACIAN_H
#define LAPLACIAN_H

#include <vector>
#include "SimpleMesh/vertex.h"
#include "SimpleMesh/triangle.h"
#include "SimpleMesh/mesh.h"
#include "HeatDiffusion/HeatDiffusion_global.h"

#ifdef _MSC_VER
    #define M_PI 3.141592
#endif

using namespace std;

namespace HeatDiffusion{

    //! Enum for types of Laplacian
    /*! Possible types of Laplacian*/
    enum LaplacianType{
        GRAPH, /*! Graph Laplacian*/
        COTAN, /*! Cotangent Laplacian*/
        BELKIN /*! Belkin Laplacian*/
    };

class Vector{

    float values[3];

public:
    Vector(float x, float y, float z){
        values[0] = x;
        values[1] = y;
        values[2] = z;
    }

    float x(){return values[0];}
    float y(){return values[1];}
    float z(){return values[2];}

    float dot(Vector b){
        return values[0] * b.x() + values[1] * b.y() + values[2] * b.z();
    }
};

//!  Laplacian class
/*!
A class to compute the Laplace-Beltrami operator of a 3D triangular mesh and its Eigendecomposition.
*/
class HEATDIFFUSION_API Laplacian
{

private:
    vector<unsigned int> IIV;
    vector<unsigned int> JJV;
    vector<float>       SSV;
    vector<float>       AAV;

    int                  type;
    float*              evals;
    float*              evecs;
    float*              areas;
    unsigned int                  num_eigenvalues;

    //float* vertex_buffer;
    //unsigned int* index_buffer;

    int num_vertex;
    int num_triangles;

    SimpleMesh::Mesh* object;

public:

    //! Default constructor
    /*!
        It creates an empty object. You must use setType and setNumberEigenvalues to provide the required information for the computation.
        \sa setType() and setNumberEigenvalues()
    */
    Laplacian();

    //! A constructor
    /*!
        It creates a Laplacian object with the given type and number of eigenvalues.
        \param t the type of Laplacian={BELKIN, COTAN, GRAPH}
        \param numEig the number of eigen-pairs in the Eigendecomposition
    */
    Laplacian(int t, unsigned int numEig){type = t; evals = evecs = areas = NULL; num_eigenvalues=numEig;}

    //! A destructor
    /*!
        Destroy the memory buffers for eigenvalues, eigenvectors and areas
    */
    ~Laplacian();

    //! Method compute
    /*!
        Method that actually performs the computation of the Laplacian matrix and the Eigendecomposition
    */
    void compute();

    //! Method to set the geometry
    /*!
        It sets the required geometry.
        \param num_vertex the number of vertices in the 3D mesh.
        \param vertexBuffer the vertex buffer (three values for each vertex).
        \param num_triangles the number of triangles in the 3D mesh.
        \param indexBuffer the index buffer (three values for each face).
    */
    void setGeometryData(int num_vertex, float* vertexBuffer, int num_triangles, unsigned int* indexBuffer);

    void setObject(SimpleMesh::Mesh *input);
    /*!
       It sets the type of Laplacian to compute.
       \param t the type of Laplacian = {BELKIN, COTAN, GRAPH}
    */
    void setType(int t){ type = t;}

    /*!
       It sets the number of eigen-pairs to compute.
       \param numEig the number of eigen-pairs required.
    */
    void setNumberEigenvalues(int numEig){num_eigenvalues = numEig;}
    //! Access method
    /*!
        Get the buffer with area elements for vertices. Hint: the buffer is destroyed when Laplacian object is done. Make a copy if needed.
    */
    float* getAreas(){return areas;}

    //! Access method
    /*!
        Get the buffer with eigenvalues. Hint: the buffer is destroyed when Laplacian object is done. Make a copy if needed.
    */
    float* getEvals(){return evals;}

    //! Access method
    /*!
        Get the buffer with eigenvectors. The eigenvectors are stored column-wise. Hint: the buffer is destroyed when Laplacian object is done. Make a copy if needed.
    */
    float* getEvecs(){return evecs;}

    //! Access method
    /*!
        Get the number of eigenvalues.
    */
    unsigned int getNumEigenvalues(){return num_eigenvalues;}

private:
    void computeBelkinLaplacian();
    void computeCotangentLaplacian();
    void computeEigendecomposition();

    float determinant(Vector a, Vector b);
    //void compute_one2part_Euclidean_vdist(unsigned int vid_start, vector<pair<unsigned int, float> >& vgdists, float maxdist);
};
}
#endif // LAPLACIAN_H
