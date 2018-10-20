#ifndef HEATDIFFUSIONSYMMETRY_H
#define HEATDIFFUSIONSYMMETRY_H

#include "heatdiffusionsymmetry_global.h"

//#include <Harris3D/Mesh.h>
//#include <Harris3D/HarrisDetector.h>
#include <SimpleMesh/mesh.h>
#include <SimpleMesh/io/offwriter.h>
//#include <Harris3D/Vertex.h>
#include <Util/PropertySet.h>
#include <Util/util.h>


//Eigen includes
#include <Eigen/IterativeLinearSolvers>

//OpenVDB includes
#include <openvdb/openvdb.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/Types.h>

#include <QThread>

//Simplification include
#include <Util/simplify.h>
#include <Util/rotation.h>

//PCL includes
#include <pcl/point_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_circle3d.h>
#include <pcl/filters/extract_indices.h>

#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/common/common.h>
#include <pcl/features/normal_3d.h>
#include <pcl/search/kdtree.h>



namespace Engine{

//This class provides a unique interface for execution
// Input:  SimpleMesh::Mesh
// Output: SimpleMesh::Mesh
class HEATDIFFUSIONSYMMETRY_API HeatDiffusionSymmetry : public QThread {

    Q_OBJECT

    SimpleMesh::Mesh*     input;
    Util::PropertySet*    prop;
    SimpleMesh::Mesh*   output;

private:

    /*Es el primer método que se llama en computeCompletion. La malla de entrada es usada como entrada original.
     * Crea una malla auxiliar que se usa en el resto de los procedimientos.
     * El método llamador tiene que hacerse cargo del objeto retornado. */
    //SimpleMesh::Mesh* applyOpenVDB();

    /*Permite reducir el número de primitivas en una malla. El objeto de entrada es el objeto que cambia*/
    //SimpleMesh::Mesh* decimate(SimpleMesh::Mesh* in);

    /*Este método sirve para transferir la información de clasificación desde la malla original a una malla de entrada.
     * El método también remueve la geometría asociada a superficies fracturadas. La nueva geometría es devuelta.
     * El método llamador tiene que hacerse cargo del objeto retornado
    */
    //SimpleMesh::Mesh* transferClassification(SimpleMesh::Mesh* in, SimpleMesh::Mesh* original);

    /*Aplica el Detector de Harris y realiza RANSAC para detectar puntos en un círculo.
     * Devuelve los parámetros del círculo*/
   // void applyRANSAC(Harris3D::Mesh* in, vector<float>& center, vector<float>& directionVector);

    /*Método que rota el objeto de entrada alrededor del círculo encontrado y alinea las mallas resultantes.
      El método llamador tiene que hacerse cargo del objeto retornado.*/
    //Harris3D::Mesh* alignMeshes(Harris3D::Mesh* in, vector<float> center, vector<float> directionVector);

    /*Los métodos a continuación son los helpers de la clase*/

    //Harris3D::Mesh* rotateMesh(Harris3D::Mesh* mesh, float angle, vector<float>& center, vector<float>& directionVector);
    //float computeSpacing(Harris3D::Mesh* mesh);
    //float computeDistance(SimpleMesh::Vertex *i, SimpleMesh::Vertex *j);
    //float computeVoronoiTriangle(Harris3D::Mesh* mesh, int i, int j);
    //float computeAreaTriangle(SimpleMesh::Vertex* i, SimpleMesh::Vertex* j, SimpleMesh::Vertex *k);
    //bool getClosestCompatiblePoint(Harris3D::Mesh* inputMesh,
    //                               int index,
    //                               Harris3D::Mesh* targetMesh,
    //                               pcl::KdTreeFLANN<pcl::PointXYZ> kdtree,
    //                               float spacing,
    //                               int& outIndex,
    //                               float factor_spacing, float compatible_angle);

    //float computeNormalDotProduct(SimpleMesh::Vertex * i, SimpleMesh::Vertex * j);
    //float computeOmega(pcl::KdTreeFLANN<pcl::PointXYZ> kdtree, SimpleMesh::Vertex* q, SimpleMesh::Vertex* v, SimpleMesh::Vertex* verts, float h);
    //float computeVoronoiCell(Harris3D::Mesh* mesh, int i);
    //Harris3D::Vertex computeBarycenter(Harris3D::Mesh* mesh, int i, int j, int k);
    //float computeVoronoiTriangle(Harris3D::Mesh *mesh, int i, int j);
    void computeCompletion();

public:
    HeatDiffusionSymmetry(QObject* parent=0);
    ~HeatDiffusionSymmetry();

    void setData(SimpleMesh::Mesh* in, Util::PropertySet* p){ input = in; prop = p;}
    SimpleMesh::Mesh* getResult(){return output;}
    void performFarthestPointSamplingSpatial(int numPoints, vector<int>& points);
    void computeAxialSupport(float nx, float ny, float nz, float cx, float cy, float cz, float threshold, vector<float>& support);
    SimpleMesh::Mesh* splitMesh(vector<float>& support, float threshold);
    SimpleMesh::Mesh* rotateAroundAxis(SimpleMesh::Mesh* mesh,float nx, float ny, float nz, float cx, float cy, float cz );



Q_SIGNALS:
    void progress(int, const QString&);

protected:
    void run();

};
} //namespace Engine
#endif // ROTATIONALSYMMETRY_H
