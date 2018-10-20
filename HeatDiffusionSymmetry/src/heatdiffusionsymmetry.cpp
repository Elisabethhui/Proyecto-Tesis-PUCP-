#include "HeatDiffusionSymmetry/heatdiffusionsymmetry.h"
//#include <ICP.h>

#include <HeatDiffusion/Laplacian.h>
#include <HeatDiffusion/Descriptor/HeatKernelSignature.h>
#include <Geodesic/geodesic.h>

#include <pcl/point_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_circle3d.h>
#include <pcl/filters/extract_indices.h>

#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/common/common.h>
#include <pcl/features/normal_3d.h>
#include <pcl/search/kdtree.h>

#include <AdaptiveClustering/cluster.h>
#include <AdaptiveClustering/adaptiveClustering.h>

namespace Engine{

typedef SimpleMesh::Vertex Vector;

class InternalPoint{
public:

    float x, y, z;
    unsigned int index;

public:
    InternalPoint(float xx, float yy, float zz, unsigned int ind){
        x = xx;
        y = yy;
        z = zz;
        index = ind;
    }
};

bool compare_points(InternalPoint a, InternalPoint b){
    return a.y < b.y;
}


HeatDiffusionSymmetry::HeatDiffusionSymmetry(QObject* parent) : QThread(parent)
{
    //We perform a deep copy of the input mesh into a Harris3D::mesh
    //input = in;
    //prop = p;
}

HeatDiffusionSymmetry::~HeatDiffusionSymmetry(){
    wait();
}


//Harris3D::Mesh* RotationalSymmetry::applyOpenVDB(){

//    cout << "RotationalSymmetry::applyOpenVDB" << endl;
//        std::vector<openvdb::Vec3s> points;
//        std::vector<openvdb::Vec3I> triangles;

//        float diag = input->get_diagonal();
//        float factor = atof(prop->getProperty("factor-volumetric").c_str());

//        openvdb::math::Transform::Ptr xform = openvdb::math::Transform::createLinearTransform(factor * diag);

//        for (int i = 0; i < input->get_number_vertices(); i++){
//            openvdb::Vec3s xyz(0.0, 0.0, 0.0);
//            xyz = xform->worldToIndex(openvdb::Vec3s(input->get_vertices()[i].x(),input->get_vertices()[i].y(),input->get_vertices()[i].z()));
//            points.push_back(xyz);
//        }
//        for (int i = 0; i < input->get_number_triangles(); i++){
//            openvdb::Vec3I triangle(input->get_triangles()[i].get_vertex_at(0),input->get_triangles()[i].get_vertex_at(1),input->get_triangles()[i].get_vertex_at(2));
//            triangles.push_back(triangle);
//        }

//        std::vector<openvdb::Vec3s> newPoints;
//        std::vector<openvdb::Vec3I> newTriangles;
//        std::vector<openvdb::Vec4I> newQuads;
//        openvdb::FloatGrid::Ptr levelset = openvdb::tools::meshToLevelSet<openvdb::FloatGrid>(*xform, points, triangles);
//        openvdb::tools::volumeToMesh(*levelset,newPoints,newTriangles,newQuads);

//        Harris3D::Mesh* newMesh = new Harris3D::Mesh();
//        //input->cleanMesh();
//        newMesh->set_number_vertices(newPoints.size());
//        newMesh->set_number_triangles(newQuads.size() * 2);

//        for (unsigned i = 0; i < newPoints.size(); i++){
//            newMesh->add_vertex(i, factor*diag*newPoints[i].x(), factor*diag*newPoints[i].y(), factor*diag*newPoints[i].z());

//        }
//        for (unsigned i = 0; i < newQuads.size(); i++){
//            newMesh->add_triangle(2*i,newQuads[i].z(),newQuads[i].y(),newQuads[i].x());
//            newMesh->add_triangle(2*i+1,newQuads[i].w(),newQuads[i].z(),newQuads[i].x());
//        }

//        newMesh->compute_normals();
//        newMesh->get_diagonal();
//        //SimpleMesh::IO::OFFWriter w("jaja.off");
//        //w.write_mesh(*newMesh);
//        return newMesh;
//}

//Harris3D::Mesh* RotationalSymmetry::decimate(Harris3D::Mesh* in){
//    Util::Simplify decimater;
//    int numFaces = atoi(prop->getProperty("num-triangles-decimation").c_str());
//    Harris3D::Mesh* out = new Harris3D::Mesh();

//    if(in->get_number_triangles()>numFaces){
//            for(int i=0;i<in->get_number_vertices();i++){
//                Util::Vertex v;
//                v.p.x=in->get_vertices()[i].x();
//                v.p.y=in->get_vertices()[i].y();
//                v.p.z=in->get_vertices()[i].z();
//                decimater.vertices.push_back(v);
//            }

//            for(int i=0;i<in->get_number_triangles();i++){
//                Util::Triangle t;
//                t.v[0] = in->get_triangles()[i].get_vertex_at(0);
//                t.v[1] = in->get_triangles()[i].get_vertex_at(1);
//                t.v[2] = in->get_triangles()[i].get_vertex_at(2);
//                decimater.triangles.push_back(t);
//            }

//            //cout << "Input: " << Simplify::triangles.size() << " triangles " << Simplify::vertices.size() << " vertices\n" << endl;

//            decimater.simplify_mesh(numFaces);

//            //cout << "Output: " << Simplify::triangles.size() << " triangles " << Simplify::vertices.size() << " vertices\n" << endl;

//            //Reconstruct mesh
//            //in->cleanMesh();
//            out->set_number_vertices(decimater.vertices.size());

//            int numFaces = 0;
//            for(unsigned int i = 0; i < decimater.triangles.size(); i++){
//                if(!decimater.triangles[i].deleted)
//                    numFaces++;
//            }
//            out->set_number_triangles(numFaces);

//            for (unsigned i = 0; i < decimater.vertices.size(); i++){
//                out->add_vertex(i, decimater.vertices[i].p.x, decimater.vertices[i].p.y, decimater.vertices[i].p.z);
//            }

//            int cont = 0;
//            for (unsigned i = 0; i < decimater.triangles.size(); i++){
//                if(!decimater.triangles[i].deleted){
//                    out->add_triangle(cont,decimater.triangles[i].v[0],decimater.triangles[i].v[1],decimater.triangles[i].v[2]);
//                    cont++;
//                }
//            }
//            decimater.vertices.clear();
//            decimater.triangles.clear();
//            out->compute_normals();
//            out->get_diagonal();
//        }else{
//            out->deepCopy(in);
//        }

//    return out;
//}

//Harris3D::Mesh* RotationalSymmetry::transferClassification(Harris3D::Mesh* in, Harris3D::Mesh *original){
//    //Crear un Kd-tree para original
//        SimpleMesh::Vertex* vertices = original->get_vertices();
//        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
//        cloud->width = original->get_number_vertices();
//        cloud->height = 1;
//        cloud->points.resize(cloud->width * cloud->height);

//        for(size_t i = 0; i < cloud->points.size(); i++){
//            cloud->points[i].x = vertices[i].x();
//            cloud->points[i].y = vertices[i].y();
//            cloud->points[i].z = vertices[i].z();
//        }

//        pcl::KdTreeFLANN<pcl::PointXYZ> tree;
//        tree.setInputCloud(cloud);

//        for(int i = 0; i < in->get_number_vertices(); i++){
//            pcl::PointXYZ sp;

//            sp.x = in->get_vertices()[i].x();
//            sp.y = in->get_vertices()[i].y();
//            sp.z = in->get_vertices()[i].z();

//            int K = 1;
//            vector<int> idx(K);
//            vector<float> dist(K);

//            if(tree.nearestKSearch(sp, K, idx, dist)>0){
//                in->get_vertices()[i].set_color(vertices[idx[0]].red(),vertices[idx[0]].green(), vertices[idx[0]].blue());
//            }
//        }

//        //Eliminar los vértices (y triangulos asociados) que estan etiquetados con azul
//        vector<bool> markVertices;
//        markVertices.resize(in->get_number_vertices(), false);

//        vector<bool> markTriangles;
//        markTriangles.resize(in->get_number_triangles(), false);

//        vector<bool> markBoundary;
//        markBoundary.resize(in->get_number_vertices(), false);

//        int numVertices = 0;
//        for(int i = 0; i < in->get_number_vertices(); i++){
//            if(in->get_vertices()[i].blue()>0.95){
//                markVertices[i] = true;
//                numVertices++;
//            }
//        }

//        int numTriangles = 0;
//        for(int i = 0; i < in->get_number_triangles(); i++){
//            vector<unsigned int> p = in->get_triangles()[i].get_vertices();
//            if(markVertices[p[0]] || markVertices[p[1]] || markVertices[p[2]]){
//                markTriangles[i] = true;
//                markBoundary[p[0]] = true;
//                markBoundary[p[1]] = true;
//                markBoundary[p[2]] = true;
//                numTriangles++;
//            }
//        }

//        numVertices = in->get_number_vertices() - numVertices;
//        numTriangles = in->get_number_triangles() - numTriangles;

//        Harris3D::Mesh* newMesh = new Harris3D::Mesh();
//        newMesh->set_number_vertices(numVertices);
//        newMesh->set_number_triangles(numTriangles);

//        vector<unsigned int> mapping;

//        unsigned int numRemoved = 0;
//        for(unsigned int i = 0; i < in->get_number_vertices(); i++){
//            if(!markVertices[i]){
//                newMesh->add_vertex(i - numRemoved, in->get_vertices()[i].x(), in->get_vertices()[i].y(), in->get_vertices()[i].z());
//                newMesh->add_normal(i - numRemoved, in->get_vertices()[i].nx(), in->get_vertices()[i].ny(), in->get_vertices()[i].nz());
//                if(markBoundary[i])
//                    newMesh->add_color(i - numRemoved, 1.0, 0.0, 0.0);
//                else
//                    newMesh->add_color(i - numRemoved, 0.0, 0.0, 0.0);

//            }else{
//                numRemoved++;
//            }
//            mapping.push_back(i - numRemoved);
//        }

//        int contTriangles = 0;
//        for(unsigned int i = 0; i < in->get_number_triangles(); i++){
//            if(!markTriangles[i]){
//                vector<unsigned int> p = in->get_triangles()[i].get_vertices();
//                newMesh->add_triangle(contTriangles++, mapping[p[0]], mapping[p[1]], mapping[p[2]]);
//            }
//        }

//        newMesh->get_diagonal();

//       return newMesh;
//}

//void RotationalSymmetry::applyRANSAC(Harris3D::Mesh* in, vector<float>& center, vector<float>& directionVector){
//        float thresholdRANSAC = atof(prop->getProperty("threshold-ransac").c_str());
//        vector<unsigned int> interestPoints;
//        Harris3D::HarrisDetector hd(in, prop);
//        hd.detectInterestPoints(interestPoints);

//        std::cout << "Interest points:" << interestPoints.size() << std::endl;
//        vector<unsigned int> filteredPoints;
//        if(in->has_color()){
//            for(unsigned int i = 0; i < interestPoints.size(); i++){
//                if(in->get_vertices()[interestPoints[i]].red() < 0.95)
//                    filteredPoints.push_back(interestPoints[i]);
//            }
//        }else{
//            filteredPoints.assign(interestPoints.begin(), interestPoints.end());
//        }

//        //Point cloud analysis
//        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

//        cloud->width    = filteredPoints.size();
//        cloud->height   = 1;
//        cloud->is_dense = false;
//        cloud->points.resize(cloud->width * cloud->height);

//        for(unsigned i = 0; i < cloud->points.size(); ++i){
//            cloud->points[i].x  = in->get_vertices()[filteredPoints[i]].x();
//            cloud->points[i].y  = in->get_vertices()[filteredPoints[i]].y();
//            cloud->points[i].z  = in->get_vertices()[filteredPoints[i]].z();

//        }

//        std::vector<int> inliers;
//        pcl::SampleConsensusModelCircle3D<pcl::PointXYZ>::Ptr model_circle(new pcl::SampleConsensusModelCircle3D<pcl::PointXYZ> (cloud));
//        pcl::RandomSampleConsensus<pcl::PointXYZ> ransac(model_circle);
//        ransac.setDistanceThreshold(thresholdRANSAC * in->get_diagonal());
//        ransac.computeModel();
//        ransac.getInliers(inliers);

//        std::cout << "Num. inliers:" << inliers.size() << std::endl;

//        //Obtain coefficients
//        Eigen::VectorXf model_coefficients;
//        ransac.getModelCoefficients(model_coefficients);

//        center.push_back(model_coefficients[0]);
//        center.push_back(model_coefficients[1]);
//        center.push_back(model_coefficients[2]);

//        directionVector.push_back(model_coefficients[4]);
//        directionVector.push_back(model_coefficients[5]);
//        directionVector.push_back(model_coefficients[6]);
//}

//Harris3D::Mesh* RotationalSymmetry::alignMeshes(Harris3D::Mesh* in, vector<float> center, vector<float> directionVector){
//        float alpha = atof(prop->getProperty("alpha").c_str());
//        int rotationFactor = atoi(prop->getProperty("rotation-factor").c_str());
//        int iter_non_rigid = atoi(prop->getProperty("iter-non-rigid").c_str());
//        float factor_spacing = atof(prop->getProperty("factor-spacing").c_str());
//        float compatible_angle = atof(prop->getProperty("compatible-angle").c_str());
//        int enable_icp = atoi(prop->getProperty("enable-icp").c_str());

//        //Computar un kdtree para la malla de entrada. Se computa solo una vez y se reutiliza siempre
//        SimpleMesh::Vertex* vertices = in->get_vertices();
//        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
//        cloud->width = in->get_number_vertices();
//        cloud->height = 1;
//        cloud->points.resize(cloud->width * cloud->height);

//        for(size_t i = 0; i < cloud->points.size(); i++){
//            cloud->points[i].x = vertices[i].x();
//            cloud->points[i].y = vertices[i].y();
//            cloud->points[i].z = vertices[i].z();
//        }

//        pcl::KdTreeFLANN<pcl::PointXYZ> treeTarget;
//        treeTarget.setInputCloud(cloud);


//        Harris3D::Mesh * bigMesh = new Harris3D::Mesh();
//        vector<SimpleMesh::Vertex> auxVertex;
//        vector<SimpleMesh::Triangle> auxTriangles;

//        int nLoop = 0;
//        for (int s=360/rotationFactor; s<360;s+=360/rotationFactor){
//            float angle = (float) s;

//            //Find rotated mesh
//            Harris3D::Mesh * auxMesh = rotateMesh(in, angle, center, directionVector);
//            float spacing = computeSpacing(auxMesh);
//            //cout << "Spacing:" <<  spacing << endl;

//            if(enable_icp==1){
//                //auxMesh contiene el resultado
//                cout << "Empieza Sparse ICP" << endl;
//                //Aplicamos SparseICP
//                Eigen::Matrix3Xd source(3, in->get_number_vertices());
//                Eigen::Matrix3Xd target(3, in->get_number_vertices());
//                Eigen::Matrix3Xd normals(3, in->get_number_vertices());

//                for(unsigned int j = 0; j < in->get_number_vertices(); j++){
//                    source(0,j) = auxMesh->get_vertices()[j].x();
//                    source(1,j) = auxMesh->get_vertices()[j].y();
//                    source(2,j) = auxMesh->get_vertices()[j].z();

//                    target(0,j) = in->get_vertices()[j].x();
//                    target(1,j) = in->get_vertices()[j].y();
//                    target(2,j) = in->get_vertices()[j].z();

//                    normals(0,j) = in->get_vertices()[j].nx();
//                    normals(1,j) = in->get_vertices()[j].ny();
//                    normals(2,j) = in->get_vertices()[j].nz();
//                }

//                ICP::Parameters par;
//                par.p = 0.5;
//                par.f = ICP::PNORM;
//                par.max_icp = 50;
//                par.max_outer = 50;

//                double a = 0;
//                ICP::point_to_plane(source, target, normals, a, par);

//                for(int j = 0; j < in->get_number_vertices(); j++){
//                    auxMesh->get_vertices()[j].setX(source(0,j));
//                    auxMesh->get_vertices()[j].setY(source(1,j));
//                    auxMesh->get_vertices()[j].setZ(source(2,j));
//                }
//            }

//            //stringstream ss;
//            //ss << "output" << nLoop << ".off";
//            //outputMesh(auxMesh, ss.str().c_str());

//            //Calculate translations
//            for (int w = 0; w < iter_non_rigid; w++){
//                //Calculamos un kdtree para la malla a procesar
//                vertices = auxMesh->get_vertices();
//                pcl::PointCloud<pcl::PointXYZ>::Ptr cloudAux (new pcl::PointCloud<pcl::PointXYZ>);
//                cloudAux->width = auxMesh->get_number_vertices();
//                cloudAux->height = 1;
//                cloudAux->points.resize(cloudAux->width * cloudAux->height);

//                for(size_t i = 0; i < cloudAux->points.size(); i++){
//                    cloudAux->points[i].x = vertices[i].x();
//                    cloudAux->points[i].y = vertices[i].y();
//                    cloudAux->points[i].z = vertices[i].z();
//                }

//                pcl::KdTreeFLANN<pcl::PointXYZ> treeSource;
//                treeSource.setInputCloud(cloudAux);

//                int n = 3 * auxMesh->get_number_vertices();
//                Eigen::VectorXf x(n), b(n);
//                Eigen::SparseMatrix<float> a(n,n);
//                x.setZero();
//                b.setZero();
//                a.setZero();
//                std::vector<Eigen::Triplet<float>> tripletList;
//                //cout << "Llenando matriz esparsa" << endl;

//                for (int j = 0; j<auxMesh->get_number_vertices(); j++){
//                    SimpleMesh::Vertex v = auxMesh->get_vertices()[j];
//                    int outIndex;

//                    float aux = 0.0;
//                    std::set<unsigned int>::iterator it;
//                    for (it = v.get_vertices().begin(); it != v.get_vertices().end(); ++it){
//                        float voronoiTriangle = computeVoronoiTriangle(auxMesh, j, *it);
//                        float edge = computeDistance(&auxMesh->get_vertices()[j], &auxMesh->get_vertices()[*it]);

//                        float coef = (2 * alpha * voronoiTriangle) / edge;
//                        aux += coef;

//                        tripletList.push_back(Eigen::Triplet<float>(3*j,3*(*it),-coef));
//                        tripletList.push_back(Eigen::Triplet<float>(3*j+1,3*(*it)+1,-coef));
//                        tripletList.push_back(Eigen::Triplet<float>(3*j+2,3*(*it)+2,-coef));
//                    }

//                    if(getClosestCompatiblePoint(auxMesh, j, in, treeTarget, spacing, outIndex, factor_spacing, compatible_angle)){
//                        SimpleMesh::Vertex q = input->get_vertices()[outIndex];
//                        float bCoef = 2*(1 - alpha)*computeOmega(treeSource, &q, &v, vertices, spacing)*computeVoronoiCell(auxMesh, j);
//                        //if(v.red()>0.95)
//                        //    bCoef /= 2;

//                        aux += bCoef;

//                        //B
//                        b(3*j) = bCoef * (q.x() - v.x());
//                        b(3*j+1) = bCoef * (q.y() - v.y());
//                        b(3*j+2) = bCoef * (q.z() - v.z());
//                    }

//                    //A
//                    tripletList.push_back(Eigen::Triplet<float>(3*j,3*j,aux));
//                    tripletList.push_back(Eigen::Triplet<float>(3*j+1,3*j+1,aux));
//                    tripletList.push_back(Eigen::Triplet<float>(3*j+2,3*j+2,aux));
//                }

//                //cout << "Optimizacion" << endl;
//                //X
//                Eigen::BiCGSTAB<Eigen::SparseMatrix<float> > solver;
//                a.setFromTriplets(tripletList.begin(), tripletList.end());
//                solver.compute(a);
//                x = solver.solve(b);

//                //Translate mesh
//                for(int i = 0; i<auxMesh->get_number_vertices(); i++){
//                    auxMesh->get_vertices()[i].setX(auxMesh->get_vertices()[i].x() + x[3*i]);
//                    auxMesh->get_vertices()[i].setY(auxMesh->get_vertices()[i].y() + x[3*i+1]);
//                    auxMesh->get_vertices()[i].setZ(auxMesh->get_vertices()[i].z() + x[3*i+2]);
//                }
//                auxMesh->compute_normals();
//            }

//            //Add vertices to newVertices
//            int offset = nLoop * in->get_number_vertices();
//            for (int i=0; i< in->get_number_vertices();i++){
//                auxVertex.push_back(SimpleMesh::Vertex(auxMesh->get_vertices()[i].x(), auxMesh->get_vertices()[i].y(), auxMesh->get_vertices()[i].z()));
//            }

//            //Obtain faces
//            int faceOffset = nLoop * in->get_number_triangles();
//            for (int i=0; i< in->get_number_triangles();i++){
//                SimpleMesh::Triangle auxFace;
//                auxFace.add_vertex(auxMesh->get_triangles()[i].get_vertex_at(0)+offset);
//                auxFace.add_vertex(auxMesh->get_triangles()[i].get_vertex_at(1)+offset);
//                auxFace.add_vertex(auxMesh->get_triangles()[i].get_vertex_at(2)+offset);
//                auxTriangles.push_back(auxFace);
//            }
//            //cout << "Loop: " << nLoop << endl;
//            nLoop++;
//            //break;
//        }

//        bigMesh->set_number_vertices(auxVertex.size());
//        bigMesh->set_number_triangles(auxTriangles.size());

//        for(int i = 0; i < auxVertex.size(); i++){
//            bigMesh->add_vertex(i, auxVertex[i].x(), auxVertex[i].y(), auxVertex[i].z());
//        }

//        for(int i = 0; i < auxTriangles.size(); i++){
//            bigMesh->add_triangle(i, auxTriangles[i].get_vertex_at(0), auxTriangles[i].get_vertex_at(1), auxTriangles[i].get_vertex_at(2));
//        }

//        return bigMesh;
//}

//Harris3D::Mesh* RotationalSymmetry::rotateMesh(Harris3D::Mesh* mesh, float angle, vector<float>& center, vector<float>& directionVector){
//    //Create the mesh
//    Harris3D::Mesh * auxMesh = new Harris3D::Mesh();
//    auxMesh->set_number_vertices(mesh->get_number_vertices());
//    auxMesh->set_number_triangles(mesh->get_number_triangles());

//    //Obtain vertices
//    for (int i=0; i< mesh->get_number_vertices();i++){
//        float * rotatedVertex = Rotation::rotateVertex(mesh->get_vertices()[i].x()-center[0],mesh->get_vertices()[i].y()-center[1],mesh->get_vertices()[i].z()-center[2],angle,directionVector[0],directionVector[1],directionVector[2]);
//        auxMesh->add_vertex(i, rotatedVertex[0]+center[0], rotatedVertex[1]+center[1], rotatedVertex[2]+center[2]);
//        auxMesh->add_color(i, mesh->get_vertices()[i].red(), mesh->get_vertices()[i].green(), mesh->get_vertices()[i].blue());
//    }

//    //Obtain faces
//    for (int i=0; i< mesh->get_number_triangles();i++){
//         auxMesh->add_triangle(i,mesh->get_triangles()[i].get_vertex_at(0),mesh->get_triangles()[i].get_vertex_at(1),mesh->get_triangles()[i].get_vertex_at(2));
//    }

//    auxMesh->compute_normals();
//    auxMesh->get_diagonal();
//    return auxMesh;
//}

////Calcular el average spacing
//float RotationalSymmetry::computeSpacing(Harris3D::Mesh* mesh){
//    int numEdges = 0;
//    float sumEdges = 0.0;

//    for(int i = 0; i < mesh->get_number_triangles(); i++){
//        sumEdges += computeDistance(&mesh->get_vertices()[mesh->get_triangles()[i].get_vertex_at(0)], &mesh->get_vertices()[mesh->get_triangles()[i].get_vertex_at(1)]);
//        sumEdges += computeDistance(&mesh->get_vertices()[mesh->get_triangles()[i].get_vertex_at(1)], &mesh->get_vertices()[mesh->get_triangles()[i].get_vertex_at(2)]);
//        sumEdges += computeDistance(&mesh->get_vertices()[mesh->get_triangles()[i].get_vertex_at(0)], &mesh->get_vertices()[mesh->get_triangles()[i].get_vertex_at(2)]);
//        numEdges += 3;
//    }

//    return sumEdges/numEdges;
//}

////Calcula la distancia entre dos puntos
//float RotationalSymmetry::computeDistance(SimpleMesh::Vertex *i, SimpleMesh::Vertex *j){

//    float sum = 0.0;
//    float d = i->x()-j->x();

//    sum += d*d;
//    d = i->y()-j->y();  sum += d*d;
//    d = i->z()-j->z();  sum += d*d;


//    return std::sqrt(sum);
//}

////Calcula triangulo Voronoi de un edge
//float RotationalSymmetry::computeVoronoiTriangle(Harris3D::Mesh* mesh, int i, int j){
//    int neighbor1 = -1;
//    int neighbor2 = -1;
//    SimpleMesh::Vertex vertex1 = mesh->get_vertices()[i];
//    SimpleMesh::Vertex vertex2 = mesh->get_vertices()[j];
//    std::set<unsigned int>::iterator it1;
//    std::set<unsigned int>::iterator it2;

//    //find if they are neighbors
//    for (it1 = vertex1.get_vertices().begin(); it1 != vertex1.get_vertices().end(); ++it1){
//        for (it2 = vertex2.get_vertices().begin(); it2 != vertex2.get_vertices().end(); ++it2){
//            if(*it1==*it2){
//                if (neighbor1 == -1){
//                    neighbor1 = *it1;
//                }
//                else{
//                    neighbor2 = *it1;
//                    break;
//                }
//            }
//        }
//    }

//    if (neighbor1!=-1 && neighbor2!=-1){
//        SimpleMesh::Vertex barycenter1 = computeBarycenter(mesh, vertex1.get_index(), vertex2.get_index(), neighbor1);
//        SimpleMesh::Vertex barycenter2 = computeBarycenter(mesh, vertex1.get_index(), vertex2.get_index(), neighbor2);

//        return computeAreaTriangle(&vertex1, &barycenter1, &barycenter2);
//    }
//    else{
//        return 0.0;
//    }
//}

////Calcula el baricentro de un triángulo dado por 3 vértices
//Harris3D::Vertex RotationalSymmetry::computeBarycenter(Harris3D::Mesh* mesh, int i, int j, int k){
//    //find barycenter
//    Harris3D::Vertex barycenter;
//    barycenter.setX((mesh->get_vertices()[i].x() + mesh->get_vertices()[j].x() + mesh->get_vertices()[k].x())/3);
//    barycenter.setY((mesh->get_vertices()[i].y() + mesh->get_vertices()[j].y() + mesh->get_vertices()[k].y())/3);
//    barycenter.setZ((mesh->get_vertices()[i].z() + mesh->get_vertices()[j].z() + mesh->get_vertices()[k].z())/3);

//    return barycenter;
//}

////Calcula el área de un triángulo
//float RotationalSymmetry::computeAreaTriangle(SimpleMesh::Vertex * i, SimpleMesh::Vertex * j, SimpleMesh::Vertex * k){
//    //Heron formula
//    float a = computeDistance(i,j);
//    float b = computeDistance(i,k);
//    float c = computeDistance(j,k);
//    float s = (a+b+c)/2;

//    return std::sqrt(s*(s-a)*(s-b)*(s-c));
//}

////Buscar en el kdtree el punto mas cercano al vertice[index] en inputMesh
//bool RotationalSymmetry::getClosestCompatiblePoint(Harris3D::Mesh *inputMesh, int index, Harris3D::Mesh* targetMesh, pcl::KdTreeFLANN<pcl::PointXYZ> kdtree, float spacing, int& outIndex, float factor_spacing, float compatible_angle){
//    outIndex = -1;
//    pcl::PointXYZ sp;
//    SimpleMesh::Vertex vertex = inputMesh->get_vertices()[index];

//    sp.x = vertex.x();
//    sp.y = vertex.y();
//    sp.z = vertex.z();

//    float radius = factor_spacing * spacing;
//    vector<int> idx;
//    vector<float> dist;

//    //Harris3D::Vertex* r = new Harris3D::Vertex;
//    SimpleMesh::Vertex candidate;

//    if(kdtree.radiusSearch(sp, radius, idx, dist)>0){
//        for(size_t i = 0; i < idx.size(); i++){
//            candidate = targetMesh->get_vertices()[idx[i]];
//            if(computeNormalDotProduct(&vertex, &candidate)>compatible_angle){
//                outIndex = idx[i];
//                break;
//            }
//        }
//    }

//    if(outIndex!=-1)
//        return true;

//    return false;

//}

////Calcula el producto interno entre dos normales
////Bug: Normales no están definidas en mallas creadas dinámicamente
//float RotationalSymmetry::computeNormalDotProduct(SimpleMesh::Vertex * i, SimpleMesh::Vertex * j){

//    return (i->nx()*j->nx()+i->ny()*j->ny()+i->nz()*j->nz());
//}

//float RotationalSymmetry::computeOmega(pcl::KdTreeFLANN<pcl::PointXYZ> kdtree, SimpleMesh::Vertex* q, SimpleMesh::Vertex* v, SimpleMesh::Vertex* verts, float h){
//    pcl::PointXYZ sp;

//    sp.x = q->x();
//    sp.y = q->y();
//    sp.z = q->z();

//    int K = 1;
//    vector<int> idx(K);
//    vector<float> dist(K);

//    SimpleMesh::Vertex* r = new SimpleMesh::Vertex;

//    if(kdtree.nearestKSearch(sp, K, idx, dist)>0){
//        r->setX(verts[idx[0]].x());
//        r->setY(verts[idx[0]].y());
//        r->setZ(verts[idx[0]].z());
//    }

//    float distF = computeDistance(v,r);
//    return std::exp(-(distF*distF)/(h * h));
//}

////Calcula celula Voronoi de un vértice
//float RotationalSymmetry::computeVoronoiCell(Harris3D::Mesh* mesh, int i){
//    float area = 0.0;
//    std::set<unsigned int>::iterator it;
//    SimpleMesh::Vertex vertex = mesh->get_vertices()[i];

//    for (it = vertex.get_vertices().begin(); it != vertex.get_vertices().end(); ++it){
//        float aux = computeVoronoiTriangle(mesh, vertex.get_index(), *it);
//        if (aux > 0.0){
//            area += aux;
//        }
//    }

//    return area;
//}

class ModelCoefficient{
public:
    float cx, cy, cz;
    float nx, ny, nz;
    float r;
    float magnitudeNormal;

    ModelCoefficient(){cx = cy = cz = nx = ny = nz = r = 0.0;}
    ModelCoefficient(float cx_, float cy_, float cz_, float nx_, float ny_, float nz_, float r_){
        cx = cx_;
        cy = cy_;
        cz = cz_;
        nx = nx_;
        ny = ny_;
        nz = nz_;
        r = r_;

        magnitudeNormal = sqrt(nx*nx + ny*ny + nz*nz);
    }

    float computeAngularDistance(const ModelCoefficient& b){
        return (1 - (nx * b.nx + ny * b.ny + nz * b.nz))/ (magnitudeNormal * b.magnitudeNormal);
    }

    float computeAxialDistance(const ModelCoefficient& b){
        float dx = cx - b.cx;
        float dy = cy - b.cy;
        float dz = cz - b.cz;

        float mag = sqrt(dx * dx + dy * dy + dz * dz);
        dx /= mag;
        dy /= mag;
        dz /= mag;

        return 1 - fabs(nx * dx + ny * dy + nz * dz);
    }
};

void HeatDiffusionSymmetry::performFarthestPointSamplingSpatial(int numPoints, vector<int> &points){
    int n = input->get_number_vertices();
    SimpleMesh::Vertex* vertices = input->get_vertices();


    int point = qrand() % n;

    float* D = new float[n];
    for(int i = 0; i < n; i++){
        float aux_x = vertices[i].x() - vertices[point].x();
        float aux_y = vertices[i].y() - vertices[point].y();
        float aux_z = vertices[i].z() - vertices[point].z();

        D[i] = sqrt(aux_x*aux_x + aux_y*aux_y + aux_z*aux_z);
    }

    float max = -1.0;
    int pos = -1;

    for(int i = 0; i < n; i++){
        if(D[i] > max){
            max = D[i];
            pos = i;
        }
    }

    points.push_back(pos);

    for(int i = 0; i < n; i++){
        float aux_x = vertices[i].x() - vertices[pos].x();
        float aux_y = vertices[i].y() - vertices[pos].y();
        float aux_z = vertices[i].z() - vertices[pos].z();

        D[i] = sqrt(aux_x*aux_x + aux_y*aux_y + aux_z*aux_z);
    }

    for(int i = 1; i < numPoints; i++){
        float max = -1.0;
        int pos = -1;

        for(int i = 0; i < n; i++){
            if(D[i] > max){
                max = D[i];
                pos = i;
            }
        }

        points.push_back(pos);

        float* Daux = new float[n];
        for(int j = 0; j < n; j++){
            float aux_x = vertices[j].x() - vertices[pos].x();
            float aux_y = vertices[j].y() - vertices[pos].y();
            float aux_z = vertices[j].z() - vertices[pos].z();

            Daux[j] = sqrt(aux_x*aux_x + aux_y*aux_y + aux_z*aux_z);
        }

        for(int i = 0; i < n; i++){
            if(Daux[i] < D[i])
                D[i] = Daux[i];
        }

        delete[] Daux;

    }

    delete[] D;

}

void HeatDiffusionSymmetry::computeCompletion(){

    SimpleMesh::Vertex* vertices = input->get_vertices();
    // 1.- Calcular la Eigen-decomposition del objeto
    HeatDiffusion::Laplacian L;
    L.setObject(input);

    emit progress(50, QString("Computing eigendecomposition"));
    L.compute();

    // 2.- Calcular HKS
    HeatDiffusion::Descriptor::HeatKernelSignature hks(L.getEvals(), L.getEvecs(), L.getAreas(),
                                                       input->get_number_vertices(), L.getNumEigenvalues(), 100);

    hks.computeDescriptor();
    float* desc = hks.getDescriptors();

    // 3.- Farthest Point Sampling
    float diag = input->get_diagonal();

    vector<int> points;
    performFarthestPointSamplingSpatial(200, points);

    cout << "Sampling performed"<< endl;


    // 4.- RANSAC + similarity search
    float* B = new float[input->get_number_vertices()];
    int num_dim = 15;

    vector< Util::SortIndex<float> > sortSet;
    vector<ModelCoefficient> C;

    for(int i = 0; i < points.size(); i++){
        if(i%4 == 0){
            emit progress(50 + i/4, QString("Detection of Rotational Symmetry"));
        }
            sortSet.clear();
            int index = points[i];

            for(int j = 0; j < input->get_number_vertices(); j++){
                float accum = 0.0;
                for(int k = 0; k < num_dim; k++){
                    accum += (desc[index*100 + k] - desc[j*100 + k]) * (desc[index*100 + k] - desc[j*100 + k]);
                }
                B[j] = accum;
                sortSet.push_back(Util::SortIndex<float>(B[j], j));
            }

         sort(sortSet.begin(), sortSet.end(), Util::SortIndex<float>());

         //Create the point cloud
         pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
         cloud->width = 100;
         cloud->height = 1;
         cloud->points.resize(cloud->width * cloud->height);


         //Populate the point cloud
         for(unsigned int i = 0; i< 100; i++){
             cloud->points[i].x = vertices[sortSet[i].index].x();
             cloud->points[i].y = vertices[sortSet[i].index].y();
             cloud->points[i].z = vertices[sortSet[i].index].z();
         }

         std::vector<int> inliers;
         pcl::SampleConsensusModelCircle3D<pcl::PointXYZ>::Ptr model_circle(new pcl::SampleConsensusModelCircle3D<pcl::PointXYZ>(cloud));
         pcl::RandomSampleConsensus<pcl::PointXYZ> ransac(model_circle);
         ransac.setDistanceThreshold(0.005 * diag);
         ransac.computeModel();
         ransac.getInliers(inliers);

        if(inliers.size()>10){ //There is enough inliers to process
            Eigen::VectorXf mc;
            ransac.getModelCoefficients(mc);

            C.push_back(ModelCoefficient(mc[0], mc[1], mc[2], mc[4], mc[5], mc[6], mc[3]));
        }
    }

    cout << "Size of C:" << C.size() << endl;
    // 5.- Symmetry detection

    // 5.1 .- Compute dissimilarity matrix
    int num_inliers = C.size();
    float* distances = new float[num_inliers * num_inliers];

    memset(distances, 0, sizeof(float)*num_inliers*num_inliers);

    for(int i = 0; i < num_inliers; i++){
        for(int j = i+1; j < num_inliers; j++){
            distances[i * num_inliers + j] = distances[j * num_inliers + i] = C[i].computeAngularDistance(C[j]);
        }
    }

    // 5.2.- Clustering
    vector<AdaptiveClustering::Cluster> clusters;
    AdaptiveClustering::adaptiveClusteringMedoids(distances, num_inliers, 0.015, 0.03, 10, 10, 0, clusters);

    // 5.3.- Retrieve the cluster with largest size and its members
    int indexClus = 0; //Stores the index of largest cluster
    int maxClus = clusters[0].getSizeIndex();

    for(int i = 1; i < clusters.size();i++){
        if(clusters[i].getSizeIndex() > maxClus){
            maxClus = clusters[i].getSizeIndex();
            indexClus = i;
        }
    }

    vector<ModelCoefficient> D;
    for(int i = 0; i < clusters[indexClus].getSizeIndex(); i++){
        D.push_back(C[clusters[indexClus].getIndex(i)]);
    }

    cout << "Size of D:" << D.size() << endl;
    delete[] distances;

    // 5.4.- Perform the second clustering on circles
    num_inliers = D.size();
    distances = new float[num_inliers * num_inliers];

    for(int i = 0; i < num_inliers; i++){
        for(int j = i + 1; j < num_inliers; j++){
            distances[i*num_inliers + j] = distances[j * num_inliers + i] = D[i].computeAxialDistance(D[j]);
        }
    }

    vector<AdaptiveClustering::Cluster> newClusters;
    AdaptiveClustering::adaptiveClusteringMedoids(distances, num_inliers, 0.25, 0.5, 5, 10, 0, newClusters);

    // 5.5.- Retrieve the final set of inliers
    indexClus = 0;
    maxClus = newClusters[0].getSizeIndex();

    for(int i = 1; i < newClusters.size(); i++){
        if(newClusters[i].getSizeIndex() > maxClus){
            maxClus = newClusters[i].getSizeIndex();
            indexClus = i;
        }
    }

    vector<ModelCoefficient> finalInliers;
    for(int i = 0; i < newClusters[indexClus].getSizeIndex(); i++){
        finalInliers.push_back(D[newClusters[indexClus].getIndex(i)]);
    }

    cout << "Final inliers done!" << endl;

    // 5.6.- Computing average circle
    ModelCoefficient averageCircle;

    for(int i = 0; i < finalInliers.size(); i++){
        averageCircle.cx += finalInliers[i].cx;
        averageCircle.cy += finalInliers[i].cy;
        averageCircle.cz += finalInliers[i].cz;

        averageCircle.nx += finalInliers[i].nx;
        averageCircle.ny += finalInliers[i].ny;
        averageCircle.nz += finalInliers[i].nz;

        averageCircle.r += finalInliers[i].r;
    }

    averageCircle.cx /= finalInliers.size();
    averageCircle.cy /= finalInliers.size();
    averageCircle.cz /= finalInliers.size();

    averageCircle.nx /= finalInliers.size();
    averageCircle.ny /= finalInliers.size();
    averageCircle.nz /= finalInliers.size();

    averageCircle.r /= finalInliers.size();

    vector<float> support;
    computeAxialSupport(averageCircle.nx, averageCircle.ny, averageCircle.nz, averageCircle.cx, averageCircle.cy, averageCircle.cz, 0.01*diag, support);

    cout << "Axial support done!" << endl;

    SimpleMesh::Mesh* auxMesh = splitMesh(support, 0.008);
    cout << "Split mesh done!" << endl;

    // 5.7.- Perform rotation of new mesh around the symmetric axis
    output = rotateAroundAxis(auxMesh, averageCircle.nx, averageCircle.ny, averageCircle.nz, averageCircle.cx, averageCircle.cy, averageCircle.cz);

    cout << "Rotate mesh done!" << endl;

    delete auxMesh;
    delete[] B;
    delete[] desc;
    delete[] distances;

    cout << "HeatDiffusion::finished" << endl;

}

SimpleMesh::Mesh* HeatDiffusionSymmetry::splitMesh(vector<float>& support, float threshold){
    SimpleMesh::Vertex* vertices = input->get_vertices();
    SimpleMesh::Triangle* triangles = input->get_triangles();

    bool* markVertex = new bool[input->get_number_vertices()];
    bool* markTriangle = new bool[input->get_number_triangles()];

    memset(markVertex, 0, sizeof(bool) * input->get_number_vertices());
    memset(markTriangle, 0, sizeof(bool) * input->get_number_triangles());

    cout << "SplitMesh: created arrays!" << endl;

    //Copiar argumento 2 en vector booleano
    for(int i = 0; i < input->get_number_vertices(); i++){
        if(support[i] > threshold)
            markVertex[i] = true;
    }

    cout << "SplitMesh: marked vertices!" << endl;

    int numMarkedTriangles = 0;
    //Marcamos triángulos que tengan tres vértices marcados
    for(int i = 0; i < input->get_number_triangles(); i++){
        if(markVertex[triangles[i].get_vertex_at(0)] && markVertex[triangles[i].get_vertex_at(1)] && markVertex[triangles[i].get_vertex_at(2)]){
            markTriangle[i] = true;
            numMarkedTriangles++;
        }
    }

    cout << "SplitMesh: marked triangles!" << endl;

    unsigned int* newIndex = new unsigned int[input->get_number_vertices()];
    memset(newIndex, 0, sizeof(unsigned int)*input->get_number_vertices());

    unsigned int numUnMarkedVertices = 0;
    unsigned int numMarkedVertices = 0;

    for(unsigned int i = 0; i < input->get_number_vertices(); i++){
        if(!markVertex[i]){
            numUnMarkedVertices++;
        }else{
            newIndex[i] = i - numUnMarkedVertices;
            numMarkedVertices++;
        }
    }

    cout << "SplitMesh: created new index!" << endl;

    unsigned int new_number_vertices = numMarkedVertices;
    unsigned int new_number_triangles = numMarkedTriangles;


    cout << "SplitMesh: " << new_number_vertices << " - " << new_number_triangles << endl;

    SimpleMesh::Mesh* result = new SimpleMesh::Mesh;
    result->set_number_vertices(new_number_vertices);
    result->set_number_triangles(new_number_triangles);

    unsigned int pos = 0;
    for(unsigned int i = 0; i < input->get_number_vertices(); i++){
        if(markVertex[i]){
            result->add_vertex(pos, vertices[i].x(), vertices[i].y(), vertices[i].z());
            pos++;
        }
    }

    pos = 0;
    for(unsigned int i = 0; i < input->get_number_triangles(); i++){
        if(markTriangle[i]){
            unsigned int v0 = triangles[i].get_vertex_at(0);
            unsigned int v1 = triangles[i].get_vertex_at(1);
            unsigned int v2 = triangles[i].get_vertex_at(2);
            result->add_triangle(pos, newIndex[v0], newIndex[v1], newIndex[v2]);
            pos++;
        }
    }

    delete[] markVertex;
    delete[] markTriangle;
    delete[] newIndex;

    return result;
}


SimpleMesh::Mesh* HeatDiffusionSymmetry::rotateAroundAxis(SimpleMesh::Mesh *mesh, float nx, float ny, float nz, float cx, float cy, float cz){
    SimpleMesh::Vertex* vertices = mesh->get_vertices();
    SimpleMesh::Triangle* triangles = mesh->get_triangles();
    int num_vertices = mesh->get_number_vertices();

    // 1.- prepare information to Eigen matrix
    Eigen::MatrixXf P(3, num_vertices);
    for(int i = 0; i < num_vertices; i++){
        P(3 * i) = vertices[i].x();
        P(3 * i + 1) = vertices[i].y();
        P(3 * i + 2) = vertices[i].z();
    }

    Eigen::Vector3f N;
    N(0) = nx;  N(1) = ny;  N(2) = nz;

    Eigen::Vector3f C;
    C(0) = cx;  C(1) = cy;  C(2) = cz;

    float L = N.dot(N);
    float angle = (120 * M_PI)/180;

    float u = N(0);
    float v = N(1);
    float w = N(2);

    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;

    Eigen::MatrixXf Rot(4,4);
    Rot(0,0) = (u2 + (v2 + w2)*cosf(angle))/L;
    Rot(0,1) = (u * v * (1 - cosf(angle)) - w * sqrtf(L) * sinf(angle)) / L;
    Rot(0,2) = (u * w * (1 - cosf(angle)) + v * sqrtf(L) * sinf(angle)) / L;
    Rot(0,3) = 0.0f;

    Rot(1,0) = (u * v * (1 - cosf(angle)) + w * sqrtf(L) * sinf(angle)) / L;
    Rot(1,1) = (v2 + (u2 + w2) * cosf(angle)) / L;
    Rot(1,2) = (v * w * (1 - cosf(angle)) - u * sqrtf(L) * sinf(angle)) / L;
    Rot(1,3) = 0.0f;

    Rot(2,0) = (u * w * (1 - cosf(angle)) - v * sqrtf(L) * sinf(angle)) / L;
    Rot(2,1) = (v * w * (1 - cosf(angle)) + u * sqrtf(L) * sinf(angle)) / L;
    Rot(2,2) = (w2 + (u2 + v2) * cosf(angle)) / L;
    Rot(2,3) = 0.0f;

    Rot(3,0) = 0.0f;
    Rot(3,1) = 0.0f;
    Rot(3,2) = 0.0f;
    Rot(3,3) = 1.0f;

    Eigen::MatrixXf T(4,4);
    T(0,0) = 1.0;   T(0,1) = 0.0;   T(0,2) = 0.0;   T(0,3) = -C(0);
    T(1,0) = 0.0;   T(1,1) = 1.0;   T(1,2) = 0.0;   T(1,3) = -C(1);
    T(2,0) = 0.0;   T(2,1) = 0.0;   T(2,2) = 1.0;   T(2,3) = -C(2);
    T(3,0) = 0.0;   T(3,1) = 0.0;   T(3,2) = 0.0;   T(3,3) = 1.0;

    Eigen::MatrixXf Tinv(4,4);
    Tinv(0,0) = 1.0;   Tinv(0,1) = 0.0;   Tinv(0,2) = 0.0;   Tinv(0,3) = C(0);
    Tinv(1,0) = 0.0;   Tinv(1,1) = 1.0;   Tinv(1,2) = 0.0;   Tinv(1,3) = C(1);
    Tinv(2,0) = 0.0;   Tinv(2,1) = 0.0;   Tinv(2,2) = 1.0;   Tinv(2,3) = C(2);
    Tinv(3,0) = 0.0;   Tinv(3,1) = 0.0;   Tinv(3,2) = 0.0;   Tinv(3,3) = 1.0;

    Eigen::MatrixXf H = Eigen::MatrixXf::Constant(4,num_vertices, 1.0);
    H.block(0,0, 3, num_vertices) = P;

    Eigen::MatrixXf newH = Tinv *Rot * T * H;
    P = newH.block(0,0, 3, num_vertices);

    SimpleMesh::Mesh* auxMesh = new SimpleMesh::Mesh;
    auxMesh->set_number_vertices(mesh->get_number_vertices());
    auxMesh->set_number_triangles(mesh->get_number_triangles());

    for(unsigned int i = 0; i < auxMesh->get_number_vertices(); i++){
        auxMesh->add_vertex(i, P(0,i), P(1,i), P(2,i));
    }

    for(unsigned int i = 0; i < auxMesh->get_number_triangles(); i++){
        auxMesh->add_triangle(i, triangles[i].get_vertex_at(0), triangles[i].get_vertex_at(1), triangles[i].get_vertex_at(2));
    }


    return auxMesh;

}
void HeatDiffusionSymmetry::computeAxialSupport(float nx, float ny, float nz, float cx, float cy, float cz, float threshold, vector<float> &support){
    SimpleMesh::Vertex* vertices = input->get_vertices();
    int num_vertices = input->get_number_vertices();

    // 1.- prepare information to Eigen matrix
    Eigen::MatrixXf P(3, num_vertices);
    for(int i = 0; i < num_vertices; i++){
        P(3 * i) = vertices[i].x();
        P(3 * i + 1) = vertices[i].y();
        P(3 * i + 2) = vertices[i].z();
    }

    Eigen::Vector3f N;
    N(0) = nx;  N(1) = ny;  N(2) = nz;

    Eigen::Vector3f C;
    C(0) = cx;  C(1) = cy;  C(2) = cz;

    Eigen::Vector3f M;
    M(0) = 0.0; M(1) = 1.0; M(2) = 0.0;

    Eigen::Vector3f V = N.cross(M);
    V = V/V.norm();

    float costheta = N.dot(M)/(N.norm() * M.norm());
    float sintheta = sqrt(1 - costheta*costheta);

    float q = 1 - costheta;

    Eigen::MatrixXf Ro(4,4);
    Ro(0,0) = q*V(0)*V(0) + costheta;   Ro(0,1) = q*V(0)*V(1) - sintheta*V(2);   Ro(0,2) = q*V(0)*V(2) + sintheta*V(1); Ro(0,3) = 0;
    Ro(1,0) = q*V(0)*V(1) + sintheta*V(2); Ro(1,1) = q*V(1)*V(1) + costheta;     Ro(1,2) = q*V(1)*V(2) - sintheta*V(0);  Ro(1,3) = 0;
    Ro(2,0) = q*V(0)*V(2) - sintheta*V(1); Ro(2,1) = q*V(1)*V(2) + sintheta*V(0);Ro(2,2) = q*V(2)*V(2) + costheta;      Ro(2,3) =0;
    Ro(3,0) = 0;    Ro(3,1) = 0;    Ro(3,2) = 0;    Ro(3,3) = 1.0;

    Eigen::MatrixXf T(4,4);
    T(0,0) = 1.0;   T(0,1) = 0.0;   T(0,2) = 0.0;   T(0,3) = -C(0);
    T(1,0) = 0.0;   T(1,1) = 1.0;   T(1,2) = 0.0;   T(1,3) = -C(1);
    T(2,0) = 0.0;   T(2,1) = 0.0;   T(2,2) = 1.0;   T(2,3) = -C(2);
    T(3,0) = 0.0;   T(3,1) = 0.0;   T(3,2) = 0.0;   T(3,3) = 1.0;

    Eigen::MatrixXf H = Eigen::MatrixXf::Constant(4,num_vertices, 1.0);
    H.block(0,0, 3, num_vertices) = P;

    Eigen::MatrixXf newH = Ro * T * H;
    P = newH.block(0,0, 3, num_vertices);

    std::vector<InternalPoint> points;
    points.reserve(num_vertices);

    for(int i = 0; i < num_vertices; i++)
        points.push_back(InternalPoint(P(0,i),P(1,i),P(2,i), i));

    sort(points.begin(), points.end(), compare_points);

    support.reserve(points.size());

    for(int i = 0; i < points.size(); i++){
        std::vector<InternalPoint>::iterator low, up;
        InternalPoint dminus(0,points[i].y-threshold,0,0);
        InternalPoint dplus(0,points[i].y+threshold,0,0);
        low = std::lower_bound(points.begin(), points.end(), dminus, compare_points);
        up  = std::upper_bound(points.begin(), points.end(), dplus, compare_points);
        //Puntos de low a up están cerca al plano en donde cae el circulo de soporte del points[i],
        //pero no necesariamente cerca al círculo en sí

        //Radio del punto es la hipotenusa de x, z
        float R = sqrt(points[i].x*points[i].x +  points[i].z*points[i].z);
        int cont = 0;
        for(std::vector<InternalPoint>::iterator it = low; it!=up; it++){
            float R1 = sqrt(it->x*it->x + it->z*it->z);
            if(fabs(R - R1) < threshold)
                cont++;
        }

        //support[i] = float(std::distance(low, up))/points.size();
        support[points[i].index] = float(cont)/points.size();
    }
}

void HeatDiffusionSymmetry::run(){
    cout << "HeatDiffusionSymmetry::run" << endl;
    computeCompletion();
}

}
