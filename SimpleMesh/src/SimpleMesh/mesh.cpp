#include <cmath>
#include "SimpleMesh/mesh.h"
#include <iostream>

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/curvature.h>

namespace SimpleMesh{



//Types used as wrappers for VCG classes

using namespace std;
using namespace Eigen;

class VCGVertex;
class VCGEdge;
class VCGFace;

struct VCGTypes: public vcg::UsedTypes<vcg::Use<VCGVertex>::AsVertexType,vcg::Use<VCGEdge>::AsEdgeType,vcg::Use<VCGFace>::AsFaceType>{};

class VCGVertex  : public vcg::Vertex< VCGTypes,
 vcg::vertex::InfoOcf,
 vcg::vertex::MarkOcf,
 vcg::vertex::VFAdj,
 vcg::vertex::Coord3f,
 vcg::vertex::Normal3f,
 vcg::vertex::Curvaturef,
 vcg::vertex::CurvatureDirf,
 vcg::vertex::Mark,
 vcg::vertex::BitFlags  >{
};

class VCGEdge : public vcg::Edge< VCGTypes> {};

class VCGFace    : public vcg::Face< VCGTypes,
 vcg::face::InfoOcf,
 vcg::face::MarkOcf,
 vcg::face::VFAdj,
 vcg::face::FFAdj,
 vcg::face::Normal3f,
 vcg::face::VertexRef,
 vcg::face::BitFlags > {};

// the main mesh class
class VCGMesh    : public vcg::tri::TriMesh<std::vector<VCGVertex>, std::vector<VCGFace> > {};


class VCGVertex_Ocf;
class VCGEdge_Ocf;
class VCGFace_Ocf;

struct VCGTypes_Ocf: public vcg::UsedTypes<vcg::Use<VCGVertex_Ocf>::AsVertexType,vcg::Use<VCGEdge_Ocf>::AsEdgeType,vcg::Use<VCGFace_Ocf>::AsFaceType>{};

class VCGVertex_Ocf  : public vcg::Vertex< VCGTypes_Ocf,
 vcg::vertex::InfoOcf,
 vcg::vertex::MarkOcf,
 vcg::vertex::VFAdjOcf,
 vcg::vertex::Coord3f,
 vcg::vertex::Normal3f,
 vcg::vertex::CurvaturefOcf,
 vcg::vertex::CurvatureDirfOcf,
 vcg::vertex::Mark,
 vcg::vertex::BitFlags  >{
};

class VCGEdge_Ocf : public vcg::Edge< VCGTypes_Ocf> {};

class VCGFace_Ocf    : public vcg::Face< VCGTypes_Ocf,
 vcg::face::InfoOcf,
 vcg::face::MarkOcf,
 vcg::face::VFAdjOcf,
 vcg::face::FFAdjOcf,
 vcg::face::Normal3f,
 vcg::face::VertexRef,
 vcg::face::BitFlags > {};

class VCGMesh_Ocf    : public vcg::tri::TriMesh<vcg::vertex::vector_ocf<VCGVertex_Ocf>, vcg::face::vector_ocf<VCGFace_Ocf> > {};

int sign(float v){
    if(v >= 0.0)
        return 1;
    else
        return -1;
}

Mesh::Mesh()
{
    vertices = 0;
    triangles = 0;
    num_vertices = 0;
    num_triangles = 0;
    diag = -1.0;
    flag_color = false;
    flag_normals = false;
}


Mesh::~Mesh()
{
    if(vertices){
        delete[] vertices;
        vertices = 0;
    }

    if(triangles){
        delete[] triangles;
        triangles = 0;
    }
}

float Mesh::get_diagonal(){

    if(diag < 0.0){
        xmin = ymin = zmin = 1.0E30F;
        xmax = ymax = zmax = -1.0E30F;

        for(int i = 0; i < num_vertices; i++){
            if(vertices[i].x() < xmin)
                xmin = vertices[i].x();
            else if(vertices[i].x() > xmax)
                xmax = vertices[i].x();

            if(vertices[i].y() < ymin)
                ymin = vertices[i].y();
            else if(vertices[i].y() > ymax)
                ymax = vertices[i].y();

            if(vertices[i].z() < zmin)
                zmin = vertices[i].z();
            else if(vertices[i].z() > zmax)
                zmax = vertices[i].z();
       }

       diag = sqrt((xmax - xmin)*(xmax - xmin) + (ymax - ymin)*(ymax - ymin) + (zmax - zmin)*(zmax - zmin));
    }

    return diag;
}

float *Mesh::GetSimetriaEstadistica(){
    int rango = 1;
    float *a = new float[3]; //Botar los resultados de la simetría
    //Plano en Z
    vector<float> planoZn;
    vector<int> indicesZ;
    int inicio = (zmin * 100000) / 100000;
    int valor = inicio / 10;
    int inicioZ = valor * 10;

    int valActualZ = inicioZ;
    cout <<inicioZ << endl;
    for(int i = inicioZ; i < zmax; i+=(rango)){
        indicesZ.clear();

        for(int j = 0; j < num_vertices; j++){
            if((valActualZ<vertices[j].z()) && ((valActualZ + rango) > vertices[j].z())){
                indicesZ.push_back(j);
            }
        }

        for(int m = 0 ; m < indicesZ.size(); m++){

        }
    }
    return a;
}

float Mesh::get_xmin(){
    return xmin;
}

float Mesh::get_xmax(){
    return xmax;
}

float Mesh::get_ymax(){
    return ymax;
}

float Mesh::get_ymin(){
    return ymin;
}

float Mesh::get_zmin(){
    return zmin;
}

float Mesh::get_zmax(){
    return zmax;
}

float Mesh::get_area(){
    float area = 0.0;

    for(int i = 0; i < num_triangles; i++){
        std::vector<unsigned int> indices = triangles[i].get_vertices();
        float x1 = vertices[indices[0]].x();
        float y1 = vertices[indices[0]].y();
        float z1 = vertices[indices[0]].z();

        float x2 = vertices[indices[1]].x();
        float y2 = vertices[indices[1]].y();
        float z2 = vertices[indices[1]].z();

        float x3 = vertices[indices[2]].x();
        float y3 = vertices[indices[2]].y();
        float z3 = vertices[indices[2]].z();

        float det1 = x1*y2 + x2*y3 + x3*y1 - x3*y2 - x2*y1 - x1*y3;
        float det2 = y1*z2 + y2*z3 + y3*z1 - y3*z2 - y2*z1 - y1*z3;
        float det3 = z1*x2 + z2*x3 + z3*x1 - z3*x2 - z2*x1 - z1*x3;

        area = area + 0.5*sqrt(det1*det1 + det2*det2 + det3*det3);
    }

    return area;
}


void Mesh::compute_normals()
{
    //Compute normals for faces
    if(flag_normals){
        return;
    }
        for(unsigned int i = 0; i < num_triangles; i++){
            float nx = 0.0, ny = 0.0, nz = 0.0;
            //std::cout << "gg wp " << i <<endl;
            vector<unsigned int> vert = triangles[i].get_vertices();
            unsigned int ind1 = 2;
            for(unsigned int j = 0; j < 3; j++){
                int ind2 = j;
                nx += (vertices[vert[ind1]].y() - vertices[vert[ind2]].y())*(vertices[vert[ind1]].z() + vertices[vert[ind2]].z());
                ny += (vertices[vert[ind1]].z() - vertices[vert[ind2]].z())*(vertices[vert[ind1]].x() + vertices[vert[ind2]].x());
                nz += (vertices[vert[ind1]].x() - vertices[vert[ind2]].x())*(vertices[vert[ind1]].y() + vertices[vert[ind2]].y());
                ind1 = ind2;
            }

            float magnitude = sqrt(nx*nx + ny*ny + nz*nz);
            if(magnitude > 1.0E-6){
                nx /= magnitude;
                ny /= magnitude;
                nz /= magnitude;
            }
            triangles[i].set_normal(nx, ny, nz);
        }

        //Compute normals for vertices
        for(unsigned int i = 0; i < num_vertices; i++){
            vector<unsigned int> fac = vertices[i].get_triangles();
            float nx = 0.0, ny = 0.0, nz = 0.0;
            for(unsigned int j = 0; j < fac.size(); j++){
                nx += triangles[fac[j]].nx();
                ny += triangles[fac[j]].ny();
                nz += triangles[fac[j]].nz();
            }

            nx /= fac.size();
            ny /= fac.size();
            nz /= fac.size();

            float magnitude = sqrt(nx*nx + ny*ny + nz*nz);
            if(magnitude > 1.0E-6){
                nx /= magnitude;
                ny /= magnitude;
                nz /= magnitude;
            }

            vertices[i].set_normal(nx, ny, nz);
        }
}

void Mesh::set_number_vertices(unsigned int nv)
{
    num_vertices = nv;
    vertices = new Vertex[num_vertices];
}

void Mesh::set_number_triangles(unsigned int nt)
{
    num_triangles = nt;
    triangles = new Triangle[num_triangles];
}

void Mesh::add_vertex(unsigned int pos, float x, float y, float z)
{
    vertices[pos].set_vertex(x, y, z);
    vertices[pos].set_index(pos);
}

void Mesh::set_vertex(unsigned int pos, float x, float y, float z)
{
    vertices[pos].setX(x);
    vertices[pos].setY(y);
    vertices[pos].setZ(z);
}

void Mesh::add_vertex(unsigned int pos, Vertex v)
{
    vertices[pos].set_vertex(v.x(), v.y(), v.z());
    vertices[pos].set_index(pos);
}

void Mesh::add_triangle(unsigned int pos, unsigned int p1, unsigned int p2, unsigned int p3)
{
    triangles[pos].add_vertex(p1);
    triangles[pos].add_vertex(p2);
    triangles[pos].add_vertex(p3);

    vertices[p1].add_triangle(pos);
    vertices[p2].add_triangle(pos);
    vertices[p3].add_triangle(pos);

    vertices[p1].add_vertex(p2);    vertices[p1].add_vertex(p3);
    vertices[p2].add_vertex(p1);    vertices[p2].add_vertex(p3);
    vertices[p3].add_vertex(p1);    vertices[p3].add_vertex(p2);

    vertices[p1].set_num_triangle(pos);     vertices[p2].set_num_triangle(pos);     vertices[p3].set_num_triangle(pos);
    vertices[p1].set_pos_barycentric(0);    vertices[p2].set_pos_barycentric(1);    vertices[p3].set_pos_barycentric(2);
}

void Mesh::add_normal(unsigned int pos, float nx, float ny, float nz)
{
    vertices[pos].set_normal(nx, ny, nz);
}

void Mesh::add_color(unsigned int pos, float r, float g, float b){
    vertices[pos].set_color(r, g, b);
}

unsigned char* Mesh::GetSimetriaX(){
    unsigned char palette[64 * 3 + 1]={
            0,	0,	143,
            0,	0,	159,
            0,	0,	175,
            0,	0,  188,
            0,	0,	207,
            0,	0,	223,
            0,	0,	239,
            0,	0,	255,
            0,	16,	255,
            0,	32,	255,
            0,	48,	255,
            0,	64,	255,
            0,	80,	255,
            0,	96,	255,
            0,	112,	255,
            0,	128,	255,
            0,	143,	255,
            0,	159,	255,
            0,	175,	255,
            0,	191,	255,
            0,	207,	255,
            0,	223,	255,
            0,	239,	255,
            0,	255,	255,
            16,	255,	239,
            32,	255,	223,
            48,	255,	207,
            64,	255,	191,
            80,	255,	175,
            96,	255,	159,
            112,	255,	143,
            128,	255,	128,
            143,	255,	112,
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            255,	239,	0,
            255,	223,	0,
            255,	207,	0,
            255,	191,	0,
            255,	175,	0,
            255,	159,	0,
            255,	143,	0,
            255,	128,	0,
            255,	112,	0,
            255,	96,	0,
            255,	80,	0,
            255,	64,	0,
            255,	48,	0,
            255,	32,	0,
            255,	16,	0,
            255,	0,	0,
            239,	0,	0,
            223,	0,	0,
            207,	0,	0,
            191,	0,	0,
            175,	0,	0,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    float offset = (xmax - xmin)/63;
    float cy = (ymax - ymin)/2;
    float cz = (zmax - zmin)/2;

    vector<Plano3D> planos;

    //Computamos todos los planos de analisis
    for(int i = 0; i < 64; i++){
        float x = xmin + i * offset;
        Plano3D pl; pl.a = 1.0; pl.d = -x;
        planos.push_back(pl);
    }

    //Computamos curvaturas
    float* K1 = new float[num_vertices];
    float* K2 = new float[num_vertices];

    VCGMesh mesh;
    VCGMesh::VertexIterator v_iter = vcg::tri::Allocator<VCGMesh>::AddVertices(mesh, num_vertices);

       for(unsigned int i = 0; i < num_vertices; i++,v_iter++)
       {
           (*v_iter).P()[0] = vertices[i].x();
           (*v_iter).P()[1] = vertices[i].y();
           (*v_iter).P()[2] = vertices[i].z();
       }

       vcg::tri::Allocator<VCGMesh>::AddFaces(mesh, num_triangles);
       for(unsigned int i = 0; i < num_triangles; i++){
           vector<unsigned int> vert = triangles[i].get_vertices();
           mesh.face[i].V(0) = &(mesh.vert[vert[0]]);
           mesh.face[i].V(1) = &(mesh.vert[vert[1]]);
           mesh.face[i].V(2) = &(mesh.vert[vert[2]]);

       }

       vcg::tri::UpdateTopology<VCGMesh>::FaceFace(mesh);
       vcg::tri::UpdateTopology<VCGMesh>::VertexFace(mesh);
       /*cout << "Valor de Manifold "<<vcg::tri::Clean<VCGMesh>::CountNonManifoldEdgeFF(mesh) << endl;
       if(vcg::tri::Clean<VCGMesh>::CountNonManifoldEdgeFF(mesh)>0){
           std::cout << "Malla no es manifold" << std::endl;
           exit(-1);
       }*/

       vcg::tri::UpdateNormal<VCGMesh>::PerVertexPerFace(mesh);
       vcg::tri::UpdateCurvature<VCGMesh>::PrincipalDirectionsNormalCycle(mesh);

      VCGMesh::VertexIterator vi;
       unsigned int cont = 0;

       for(vi = mesh.vert.begin(); vi!=mesh.vert.end(); ++vi){
           K1[cont] = (*vi).K1();
           K2[cont] = (*vi).K2();
           cont++;
       }

       //Para cada plano, calcular cuanta simetría genera
       vector<float> votos;
       for(int i = 0; i < planos.size(); i++){
           float val = computar_votos(planos[i], K1, K2);
           votos.push_back(val);
       }

       //Normalizar el vector de contribuciones
      float maxvote = *std::max_element(votos.begin(), votos.end());
      cout << "Maximo valor de x " << maxvote << endl;
      for(int i = 0; i < votos.size(); i++)
          votos[i] = votos[i]/maxvote;

      unsigned char* mapcolor = new unsigned char[64 * 3 + 1];

      for(int i = 0; i < votos.size(); i++){
          int index = floor(votos[i] * 63);
          mapcolor[3*i] = palette[3*index];
          mapcolor[3*i+1] = palette[3*index+1];
          mapcolor[3*i+2] = palette[3*index+2];
      }

      return mapcolor;
}

unsigned char* Mesh::GetSimetriaY(){
    unsigned char palette[64 * 3 + 1]={
            0,	0,	143,
            0,	0,	159,
            0,	0,	175,
            0,	0,  188,
            0,	0,	207,
            0,	0,	223,
            0,	0,	239,
            0,	0,	255,
            0,	16,	255,
            0,	32,	255,
            0,	48,	255,
            0,	64,	255,
            0,	80,	255,
            0,	96,	255,
            0,	112,	255,
            0,	128,	255,
            0,	143,	255,
            0,	159,	255,
            0,	175,	255,
            0,	191,	255,
            0,	207,	255,
            0,	223,	255,
            0,	239,	255,
            0,	255,	255,
            16,	255,	239,
            32,	255,	223,
            48,	255,	207,
            64,	255,	191,
            80,	255,	175,
            96,	255,	159,
            112,	255,	143,
            128,	255,	128,
            143,	255,	112,
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            255,	239,	0,
            255,	223,	0,
            255,	207,	0,
            255,	191,	0,
            255,	175,	0,
            255,	159,	0,
            255,	143,	0,
            255,	128,	0,
            255,	112,	0,
            255,	96,	0,
            255,	80,	0,
            255,	64,	0,
            255,	48,	0,
            255,	32,	0,
            255,	16,	0,
            255,	0,	0,
            239,	0,	0,
            223,	0,	0,
            207,	0,	0,
            191,	0,	0,
            175,	0,	0,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    float offset = (ymax - ymin)/63;
    float cy = (ymax - ymin)/2;
    float cz = (zmax - zmin)/2;

    vector<Plano3D> planos;

    //Computamos todos los planos de analisis
    for(int i = 0; i < 64; i++){
        float x = ymin + i * offset;
        Plano3D pl; pl.b = 1.0; pl.d = -x;
        planos.push_back(pl);
    }

    //Computamos curvaturas
    float* K1 = new float[num_vertices];
    float* K2 = new float[num_vertices];

    VCGMesh mesh;
    VCGMesh::VertexIterator v_iter = vcg::tri::Allocator<VCGMesh>::AddVertices(mesh, num_vertices);

       for(unsigned int i = 0; i < num_vertices; i++,v_iter++)
       {
           (*v_iter).P()[0] = vertices[i].x();
           (*v_iter).P()[1] = vertices[i].y();
           (*v_iter).P()[2] = vertices[i].z();
       }

       vcg::tri::Allocator<VCGMesh>::AddFaces(mesh, num_triangles);
       for(unsigned int i = 0; i < num_triangles; i++){
           vector<unsigned int> vert = triangles[i].get_vertices();
           mesh.face[i].V(0) = &(mesh.vert[vert[0]]);
           mesh.face[i].V(1) = &(mesh.vert[vert[1]]);
           mesh.face[i].V(2) = &(mesh.vert[vert[2]]);

       }

       vcg::tri::UpdateTopology<VCGMesh>::FaceFace(mesh);
       vcg::tri::UpdateTopology<VCGMesh>::VertexFace(mesh);

       vcg::tri::UpdateNormal<VCGMesh>::PerVertexPerFace(mesh);
       vcg::tri::UpdateCurvature<VCGMesh>::PrincipalDirectionsNormalCycle(mesh);

      VCGMesh::VertexIterator vi;
       unsigned int cont = 0;

       for(vi = mesh.vert.begin(); vi!=mesh.vert.end(); ++vi){
           K1[cont] = (*vi).K1();
           K2[cont] = (*vi).K2();
           cont++;
       }

       //Para cada plano, calcular cuanta simetría genera
       vector<float> votos;
       for(int i = 0; i < planos.size(); i++){
           float val = computar_votos(planos[i], K1, K2);
           votos.push_back(val);
       }

       //Normalizar el vector de contribuciones
      float maxvote = *std::max_element(votos.begin(), votos.end());
      cout << "Maximo valor de y " << maxvote << endl;
      for(int i = 0; i < votos.size(); i++)
          votos[i] = votos[i]/maxvote;

      unsigned char* mapcolor = new unsigned char[64 * 3 + 1];

      for(int i = 0; i < votos.size(); i++){
          int index = floor(votos[i] * 63);
          mapcolor[3*i] = palette[3*index];
          mapcolor[3*i+1] = palette[3*index+1];
          mapcolor[3*i+2] = palette[3*index+2];
      }

      return mapcolor;
}

unsigned char* Mesh::GetSimetriaZ(){
    unsigned char palette[64 * 3 + 1]={
            0,	0,	143,
            0,	0,	159,
            0,	0,	175,
            0,	0,  188,
            0,	0,	207,
            0,	0,	223,
            0,	0,	239,
            0,	0,	255,
            0,	16,	255,
            0,	32,	255,
            0,	48,	255,
            0,	64,	255,
            0,	80,	255,
            0,	96,	255,
            0,	112,	255,
            0,	128,	255,
            0,	143,	255,
            0,	159,	255,
            0,	175,	255,
            0,	191,	255,
            0,	207,	255,
            0,	223,	255,
            0,	239,	255,
            0,	255,	255,
            16,	255,	239,
            32,	255,	223,
            48,	255,	207,
            64,	255,	191,
            80,	255,	175,
            96,	255,	159,
            112,	255,	143,
            128,	255,	128,
            143,	255,	112,
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            255,	239,	0,
            255,	223,	0,
            255,	207,	0,
            255,	191,	0,
            255,	175,	0,
            255,	159,	0,
            255,	143,	0,
            255,	128,	0,
            255,	112,	0,
            255,	96,	0,
            255,	80,	0,
            255,	64,	0,
            255,	48,	0,
            255,	32,	0,
            255,	16,	0,
            255,	0,	0,
            239,	0,	0,
            223,	0,	0,
            207,	0,	0,
            191,	0,	0,
            175,	0,	0,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    float offset = (zmax - zmin)/63;
    float cy = (ymax - ymin)/2;
    float cz = (zmax - zmin)/2;

    vector<Plano3D> planos;

    //Computamos todos los planos de analisis
    for(int i = 0; i < 64; i++){
        float x = zmin + i * offset;
        Plano3D pl; pl.c = 1.0; pl.d = -x;
        planos.push_back(pl);
    }

    //Computamos curvaturas
    float* K1 = new float[num_vertices];
    float* K2 = new float[num_vertices];

    VCGMesh mesh;
    VCGMesh::VertexIterator v_iter = vcg::tri::Allocator<VCGMesh>::AddVertices(mesh, num_vertices);

       for(unsigned int i = 0; i < num_vertices; i++,v_iter++)
       {
           (*v_iter).P()[0] = vertices[i].x();
           (*v_iter).P()[1] = vertices[i].y();
           (*v_iter).P()[2] = vertices[i].z();
       }

       vcg::tri::Allocator<VCGMesh>::AddFaces(mesh, num_triangles);
       for(unsigned int i = 0; i < num_triangles; i++){
           vector<unsigned int> vert = triangles[i].get_vertices();
           mesh.face[i].V(0) = &(mesh.vert[vert[0]]);
           mesh.face[i].V(1) = &(mesh.vert[vert[1]]);
           mesh.face[i].V(2) = &(mesh.vert[vert[2]]);

       }

       vcg::tri::UpdateTopology<VCGMesh>::FaceFace(mesh);
       vcg::tri::UpdateTopology<VCGMesh>::VertexFace(mesh);

       vcg::tri::UpdateNormal<VCGMesh>::PerVertexPerFace(mesh);
       vcg::tri::UpdateCurvature<VCGMesh>::PrincipalDirectionsNormalCycle(mesh);

      VCGMesh::VertexIterator vi;
       unsigned int cont = 0;

       for(vi = mesh.vert.begin(); vi!=mesh.vert.end(); ++vi){
           K1[cont] = (*vi).K1();
           K2[cont] = (*vi).K2();
           cont++;
       }

       //Para cada plano, calcular cuanta simetría genera
       vector<float> votos;
       for(int i = 0; i < planos.size(); i++){
           float val = computar_votos(planos[i], K1, K2);
           votos.push_back(val);
       }

       //Normalizar el vector de contribuciones
      float maxvote = *std::max_element(votos.begin(), votos.end());
      cout << "Maximo valor de z " << maxvote << endl;
      for(int i = 0; i < votos.size(); i++)
          votos[i] = votos[i]/maxvote;

      unsigned char* mapcolor = new unsigned char[64 * 3 + 1];

      for(int i = 0; i < votos.size(); i++){
          int index = floor(votos[i] * 63);
          mapcolor[3*i] = palette[3*index];
          mapcolor[3*i+1] = palette[3*index+1];
          mapcolor[3*i+2] = palette[3*index+2];
      }

      return mapcolor;
}
float Mesh::computar_votos( Plano3D pl, float *K1, float *K2){

    float curv = 0.8;
    float vote = 0.0;
    float beta = 0.4;

       //Compute distance of every point to plane
       vector<float> distSupport(num_vertices, 0.0);
       for(unsigned int j = 0; j < num_vertices; j++){
           distSupport[j] = pl.getA()*vertices[j].x() + pl.getB()*vertices[j].y() + pl.getC()*vertices[j].z() + pl.getD();
       }

       //Compute the support of the plane
       float min_sup = distSupport[0];
       float max_sup = distSupport[0];
       for(unsigned int j = 1; j < num_vertices; j++){
           if(distSupport[j] < min_sup)
               min_sup = distSupport[j];
           if(distSupport[j] > max_sup)
               max_sup = distSupport[j];
       }

       float max_dist = fabs(max_sup) < fabs(min_sup)? fabs(max_sup) : fabs(min_sup);

       //Selecting the sample points
       vector<unsigned int> samples;
       for(unsigned int j = 0; j < num_vertices; j++){
           if((K1[j] + K2[j]) > curv && distSupport[j] < max_dist)
               samples.push_back(j);
       }

       if(samples.size()==0)
           return 0.0;

       //The votes take place here
       for(unsigned int j = 0; j < (samples.size()-1); j++){
           for(unsigned int k = j+1; k < samples.size(); k++){
               int test1 = sign(distSupport[samples[j]]);
               int test2 = sign(distSupport[samples[k]]);

               if(test1 == test2)
                   continue;

               int test5 = sign(pl.getA()*vertices[samples[j]].nx() + pl.getB()*vertices[samples[j]].ny() + pl.getC()*vertices[samples[j]].nz());
               int test6 = sign(pl.getA()*vertices[samples[k]].nx() + pl.getB()*vertices[samples[k]].ny() + pl.getC()*vertices[samples[k]].nz());

               if(test5 == test6)
                   continue;

               //float test7 = fc.evaluate_HKS_distance(samples[j], samples[k], 15);
               //if(test7 > feat)
               //    continue;

               //float test3 = fabs(fabs(distSupport[samples[j]]) - fabs(distSupport[samples[k]]));
               //if(test3 > theta)
               //    continue;

               Vertex N(vertices[samples[j]].x() - vertices[samples[k]].x(),
                       vertices[samples[j]].y() - vertices[samples[k]].y(),
                       vertices[samples[j]].z() - vertices[samples[k]].z());

               float magnitude = sqrt(N.x()*N.x() + N.y()*N.y() + N.z()*N.z());
               N.setX(N.x()/magnitude);  N.setY(N.y()/magnitude);  N.setZ(N.z()/magnitude);


               float test4 = 1 - fabs(pl.getA()*N.x() + pl.getB()*N.y() + pl.getC()*N.z());

               if(test4 > beta)
                   continue;

               vote += exp(-test4*test4);
           }
       }

       return vote;

}


vector<float> Mesh::GetSimetriaXValues(){
    unsigned char palette[64 * 3 + 1]={
            0,	0,	143,
            0,	0,	159,
            0,	0,	175,
            0,	0,  188,
            0,	0,	207,
            0,	0,	223,
            0,	0,	239,
            0,	0,	255,
            0,	16,	255,
            0,	32,	255,
            0,	48,	255,
            0,	64,	255,
            0,	80,	255,
            0,	96,	255,
            0,	112,	255,
            0,	128,	255,
            0,	143,	255,
            0,	159,	255,
            0,	175,	255,
            0,	191,	255,
            0,	207,	255,
            0,	223,	255,
            0,	239,	255,
            0,	255,	255,
            16,	255,	239,
            32,	255,	223,
            48,	255,	207,
            64,	255,	191,
            80,	255,	175,
            96,	255,	159,
            112,	255,	143,
            128,	255,	128,
            143,	255,	112,
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            255,	239,	0,
            255,	223,	0,
            255,	207,	0,
            255,	191,	0,
            255,	175,	0,
            255,	159,	0,
            255,	143,	0,
            255,	128,	0,
            255,	112,	0,
            255,	96,	0,
            255,	80,	0,
            255,	64,	0,
            255,	48,	0,
            255,	32,	0,
            255,	16,	0,
            255,	0,	0,
            239,	0,	0,
            223,	0,	0,
            207,	0,	0,
            191,	0,	0,
            175,	0,	0,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    float offset = (xmax - xmin)/63;
    float cy = (ymax - ymin)/2;
    float cz = (zmax - zmin)/2;

    vector<Plano3D> planos;

    //Computamos todos los planos de analisis
    for(int i = 0; i < 64; i++){
        float x = xmin + i * offset;
        Plano3D pl; pl.a = 1.0; pl.d = -x;
        planos.push_back(pl);
    }

    //Computamos curvaturas
    float* K1 = new float[num_vertices];
    float* K2 = new float[num_vertices];

    VCGMesh mesh;
    VCGMesh::VertexIterator v_iter = vcg::tri::Allocator<VCGMesh>::AddVertices(mesh, num_vertices);

       for(unsigned int i = 0; i < num_vertices; i++,v_iter++)
       {
           (*v_iter).P()[0] = vertices[i].x();
           (*v_iter).P()[1] = vertices[i].y();
           (*v_iter).P()[2] = vertices[i].z();
       }

       vcg::tri::Allocator<VCGMesh>::AddFaces(mesh, num_triangles);
       for(unsigned int i = 0; i < num_triangles; i++){
           vector<unsigned int> vert = triangles[i].get_vertices();
           mesh.face[i].V(0) = &(mesh.vert[vert[0]]);
           mesh.face[i].V(1) = &(mesh.vert[vert[1]]);
           mesh.face[i].V(2) = &(mesh.vert[vert[2]]);

       }

       vcg::tri::UpdateTopology<VCGMesh>::FaceFace(mesh);
       vcg::tri::UpdateTopology<VCGMesh>::VertexFace(mesh);
       /*cout << "Valor de Manifold "<<vcg::tri::Clean<VCGMesh>::CountNonManifoldEdgeFF(mesh) << endl;
       if(vcg::tri::Clean<VCGMesh>::CountNonManifoldEdgeFF(mesh)>0){
           std::cout << "Malla no es manifold" << std::endl;
           exit(-1);
       }*/

       vcg::tri::UpdateNormal<VCGMesh>::PerVertexPerFace(mesh);
       vcg::tri::UpdateCurvature<VCGMesh>::PrincipalDirectionsNormalCycle(mesh);

      VCGMesh::VertexIterator vi;
       unsigned int cont = 0;

       for(vi = mesh.vert.begin(); vi!=mesh.vert.end(); ++vi){
           K1[cont] = (*vi).K1();
           K2[cont] = (*vi).K2();
           cont++;
       }

       //Para cada plano, calcular cuanta simetría genera
       vector<float> votos;
       float maxValue = -10000.0;
       for(int i = 0; i < planos.size(); i++){
           this->planosX.push_back(planos[i]);
           float val = computar_votos(planos[i], K1, K2);
           if(val >= maxValue){
               this->mejorPlanoX = planos[i];
               this->indiceMejorPlanoX = i;
               maxValue = val;
           }
           votos.push_back(val);
       }

       //Normalizar el vector de contribuciones
      /*float maxvote = *std::max_element(votos.begin(), votos.end());
      cout << "Maximo valor de x " << maxvote << endl;
      for(int i = 0; i < votos.size(); i++)
          votos[i] = votos[i]/maxvote;

      unsigned char* mapcolor = new unsigned char[64 * 3 + 1];

      for(int i = 0; i < votos.size(); i++){
          int index = floor(votos[i] * 63);
          mapcolor[3*i] = palette[3*index];
          mapcolor[3*i+1] = palette[3*index+1];
          mapcolor[3*i+2] = palette[3*index+2];
      }

      return mapcolor;*/
      return votos;
}

vector<float> Mesh::GetSimetriaYValues(){
    unsigned char palette[64 * 3 + 1]={
            0,	0,	143,
            0,	0,	159,
            0,	0,	175,
            0,	0,  188,
            0,	0,	207,
            0,	0,	223,
            0,	0,	239,
            0,	0,	255,
            0,	16,	255,
            0,	32,	255,
            0,	48,	255,
            0,	64,	255,
            0,	80,	255,
            0,	96,	255,
            0,	112,	255,
            0,	128,	255,
            0,	143,	255,
            0,	159,	255,
            0,	175,	255,
            0,	191,	255,
            0,	207,	255,
            0,	223,	255,
            0,	239,	255,
            0,	255,	255,
            16,	255,	239,
            32,	255,	223,
            48,	255,	207,
            64,	255,	191,
            80,	255,	175,
            96,	255,	159,
            112,	255,	143,
            128,	255,	128,
            143,	255,	112,
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            255,	239,	0,
            255,	223,	0,
            255,	207,	0,
            255,	191,	0,
            255,	175,	0,
            255,	159,	0,
            255,	143,	0,
            255,	128,	0,
            255,	112,	0,
            255,	96,	0,
            255,	80,	0,
            255,	64,	0,
            255,	48,	0,
            255,	32,	0,
            255,	16,	0,
            255,	0,	0,
            239,	0,	0,
            223,	0,	0,
            207,	0,	0,
            191,	0,	0,
            175,	0,	0,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    float offset = (ymax - ymin)/63;
    float cy = (ymax - ymin)/2;
    float cz = (zmax - zmin)/2;

    vector<Plano3D> planos;

    //Computamos todos los planos de analisis
    for(int i = 0; i < 64; i++){
        float x = ymin + i * offset;
        Plano3D pl; pl.b = 1.0; pl.d = -x;
        planos.push_back(pl);
    }

    //Computamos curvaturas
    float* K1 = new float[num_vertices];
    float* K2 = new float[num_vertices];

    VCGMesh mesh;
    VCGMesh::VertexIterator v_iter = vcg::tri::Allocator<VCGMesh>::AddVertices(mesh, num_vertices);

       for(unsigned int i = 0; i < num_vertices; i++,v_iter++)
       {
           (*v_iter).P()[0] = vertices[i].x();
           (*v_iter).P()[1] = vertices[i].y();
           (*v_iter).P()[2] = vertices[i].z();
       }

       vcg::tri::Allocator<VCGMesh>::AddFaces(mesh, num_triangles);
       for(unsigned int i = 0; i < num_triangles; i++){
           vector<unsigned int> vert = triangles[i].get_vertices();
           mesh.face[i].V(0) = &(mesh.vert[vert[0]]);
           mesh.face[i].V(1) = &(mesh.vert[vert[1]]);
           mesh.face[i].V(2) = &(mesh.vert[vert[2]]);

       }

       vcg::tri::UpdateTopology<VCGMesh>::FaceFace(mesh);
       vcg::tri::UpdateTopology<VCGMesh>::VertexFace(mesh);

       vcg::tri::UpdateNormal<VCGMesh>::PerVertexPerFace(mesh);
       vcg::tri::UpdateCurvature<VCGMesh>::PrincipalDirectionsNormalCycle(mesh);

      VCGMesh::VertexIterator vi;
       unsigned int cont = 0;

       for(vi = mesh.vert.begin(); vi!=mesh.vert.end(); ++vi){
           K1[cont] = (*vi).K1();
           K2[cont] = (*vi).K2();
           cont++;
       }

       //Para cada plano, calcular cuanta simetría genera
       vector<float> votos;
       float maxValue = -10000.0;
       for(int i = 0; i < planos.size(); i++){
           this->planosY.push_back(planos[i]);
           float val = computar_votos(planos[i], K1, K2);
           if(val >= maxValue){
               this->mejorPlanoY = planos[i];
               this->indiceMejorPlanoY = i;
               maxValue = val;
           }
           votos.push_back(val);
       }

       //Normalizar el vector de contribuciones
      /*float maxvote = *std::max_element(votos.begin(), votos.end());
      cout << "Maximo valor de y " << maxvote << endl;
      for(int i = 0; i < votos.size(); i++)
          votos[i] = votos[i]/maxvote;

      unsigned char* mapcolor = new unsigned char[64 * 3 + 1];

      for(int i = 0; i < votos.size(); i++){
          int index = floor(votos[i] * 63);
          mapcolor[3*i] = palette[3*index];
          mapcolor[3*i+1] = palette[3*index+1];
          mapcolor[3*i+2] = palette[3*index+2];
      }

      return mapcolor;*/
      return votos;
}

vector<float> Mesh::GetSimetriaZValues(){
    unsigned char palette[64 * 3 + 1]={
            0,	0,	143,
            0,	0,	159,
            0,	0,	175,
            0,	0,  188,
            0,	0,	207,
            0,	0,	223,
            0,	0,	239,
            0,	0,	255,
            0,	16,	255,
            0,	32,	255,
            0,	48,	255,
            0,	64,	255,
            0,	80,	255,
            0,	96,	255,
            0,	112,	255,
            0,	128,	255,
            0,	143,	255,
            0,	159,	255,
            0,	175,	255,
            0,	191,	255,
            0,	207,	255,
            0,	223,	255,
            0,	239,	255,
            0,	255,	255,
            16,	255,	239,
            32,	255,	223,
            48,	255,	207,
            64,	255,	191,
            80,	255,	175,
            96,	255,	159,
            112,	255,	143,
            128,	255,	128,
            143,	255,	112,
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            255,	239,	0,
            255,	223,	0,
            255,	207,	0,
            255,	191,	0,
            255,	175,	0,
            255,	159,	0,
            255,	143,	0,
            255,	128,	0,
            255,	112,	0,
            255,	96,	0,
            255,	80,	0,
            255,	64,	0,
            255,	48,	0,
            255,	32,	0,
            255,	16,	0,
            255,	0,	0,
            239,	0,	0,
            223,	0,	0,
            207,	0,	0,
            191,	0,	0,
            175,	0,	0,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    float offset = (zmax - zmin)/63;
    float cy = (ymax - ymin)/2;
    float cz = (zmax - zmin)/2;

    vector<Plano3D> planos;

    //Computamos todos los planos de analisis
    for(int i = 0; i < 64; i++){
        float x = zmin + i * offset;
        Plano3D pl; pl.c = 1.0; pl.d = -x;
        planos.push_back(pl);
    }

    //Computamos curvaturas
    float* K1 = new float[num_vertices];
    float* K2 = new float[num_vertices];

    VCGMesh mesh;
    VCGMesh::VertexIterator v_iter = vcg::tri::Allocator<VCGMesh>::AddVertices(mesh, num_vertices);

       for(unsigned int i = 0; i < num_vertices; i++,v_iter++)
       {
           (*v_iter).P()[0] = vertices[i].x();
           (*v_iter).P()[1] = vertices[i].y();
           (*v_iter).P()[2] = vertices[i].z();
       }

       vcg::tri::Allocator<VCGMesh>::AddFaces(mesh, num_triangles);
       for(unsigned int i = 0; i < num_triangles; i++){
           vector<unsigned int> vert = triangles[i].get_vertices();
           mesh.face[i].V(0) = &(mesh.vert[vert[0]]);
           mesh.face[i].V(1) = &(mesh.vert[vert[1]]);
           mesh.face[i].V(2) = &(mesh.vert[vert[2]]);

       }

       vcg::tri::UpdateTopology<VCGMesh>::FaceFace(mesh);
       vcg::tri::UpdateTopology<VCGMesh>::VertexFace(mesh);

       vcg::tri::UpdateNormal<VCGMesh>::PerVertexPerFace(mesh);
       vcg::tri::UpdateCurvature<VCGMesh>::PrincipalDirectionsNormalCycle(mesh);

      VCGMesh::VertexIterator vi;
       unsigned int cont = 0;

       for(vi = mesh.vert.begin(); vi!=mesh.vert.end(); ++vi){
           K1[cont] = (*vi).K1();
           K2[cont] = (*vi).K2();
           cont++;
       }

       //Para cada plano, calcular cuanta simetría genera
       vector<float> votos;
       float maxValue = -10000.0;
       for(int i = 0; i < planos.size(); i++){
           this->planosZ.push_back(planos[i]);
           float val = computar_votos(planos[i], K1, K2);
           if(val >= maxValue){
               this->mejorPlanoZ = planos[i];
               this->indiceMejorPlanoZ = i;
               maxValue = val;
           }
           votos.push_back(val);
       }



       //Normalizar el vector de contribuciones
      /*float maxvote = *std::max_element(votos.begin(), votos.end());
      cout << "Maximo valor de z " << maxvote << endl;
      for(int i = 0; i < votos.size(); i++)
          votos[i] = votos[i]/maxvote;

      unsigned char* mapcolor = new unsigned char[64 * 3 + 1];

      for(int i = 0; i < votos.size(); i++){
          int index = floor(votos[i] * 63);
          mapcolor[3*i] = palette[3*index];
          mapcolor[3*i+1] = palette[3*index+1];
          mapcolor[3*i+2] = palette[3*index+2];
      }

      return mapcolor;*/
      return votos;
}


void Mesh::ComputeBarsSymmetry(unsigned char* mapcolorX,
                               unsigned char* mapcolorY,
                               unsigned char* mapcolorZ,
                               int &indiceX, int &indiceY, int &indiceZ){
    unsigned char palette[64 * 3 + 1]={
            0,	0,	143,
            0,	0,	159,
            0,	0,	175,
            0,	0,  188,
            0,	0,	207,
            0,	0,	223,
            0,	0,	239,
            0,	0,	255,
            0,	16,	255,
            0,	32,	255,
            0,	48,	255,
            0,	64,	255,
            0,	80,	255,
            0,	96,	255,
            0,	112,	255,
            0,	128,	255,
            0,	143,	255,
            0,	159,	255,
            0,	175,	255,
            0,	191,	255,
            0,	207,	255,
            0,	223,	255,
            0,	239,	255,
            0,	255,	255,
            16,	255,	239,
            32,	255,	223,
            48,	255,	207,
            64,	255,	191,
            80,	255,	175,
            96,	255,	159,
            112,	255,	143,
            128,	255,	128,
            143,	255,	112,
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            255,	239,	0,
            255,	223,	0,
            255,	207,	0,
            255,	191,	0,
            255,	175,	0,
            255,	159,	0,
            255,	143,	0,
            255,	128,	0,
            255,	112,	0,
            255,	96,	0,
            255,	80,	0,
            255,	64,	0,
            255,	48,	0,
            255,	32,	0,
            255,	16,	0,
            255,	0,	0,
            239,	0,	0,
            223,	0,	0,
            207,	0,	0,
            191,	0,	0,
            175,	0,	0,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    vector<float> datos_X = GetSimetriaXValues();
    vector<float> datos_Y = GetSimetriaYValues();
    vector<float> datos_Z = GetSimetriaZValues();

    float maxvoteX = *std::max_element(datos_X.begin(), datos_X.end());
    float maxvoteY = *std::max_element(datos_Y.begin(), datos_Y.end());
    float maxvoteZ = *std::max_element(datos_Z.begin(), datos_Z.end());

    float maxvote = 1.0;
    if(maxvoteX >= maxvoteZ && maxvoteX >= maxvoteY) maxvote = maxvoteX;
    else if(maxvoteY >= maxvoteX && maxvoteY >= maxvoteZ) maxvote = maxvoteY;
    else if(maxvoteZ >= maxvoteX && maxvoteZ >= maxvoteY) maxvote = maxvoteZ;

    for(int i = 0; i < datos_X.size(); i++)
        datos_X[i] = datos_X[i]/maxvote;

    for(int i = 0; i < datos_Y.size(); i++)
        datos_Y[i] = datos_Y[i]/maxvote;

    for(int i = 0; i < datos_Y.size(); i++)
        datos_Z[i] = datos_Z[i]/maxvote;

    /*unsigned char* mapcolorX = new unsigned char[64 * 3 + 1];
    unsigned char* mapcolorY = new unsigned char[64 * 3 + 1];
    unsigned char* mapcolorZ = new unsigned char[64 * 3 + 1];*/

    for(int i = 0; i < datos_X.size(); i++){
        int index = floor(datos_X[i] * 63);
        mapcolorX[3*i] = palette[3*index];
        mapcolorX[3*i+1] = palette[3*index+1];
        mapcolorX[3*i+2] = palette[3*index+2];
    }

    for(int i = 0; i < datos_Y.size(); i++){
        int index = floor(datos_Y[i] * 63);
        mapcolorY[3*i] = palette[3*index];
        mapcolorY[3*i+1] = palette[3*index+1];
        mapcolorY[3*i+2] = palette[3*index+2];
    }

    for(int i = 0; i < datos_Z.size(); i++){
        int index = floor(datos_Z[i] * 63);
        mapcolorZ[3*i] = palette[3*index];
        mapcolorZ[3*i+1] = palette[3*index+1];
        mapcolorZ[3*i+2] = palette[3*index+2];
    }

    //Seteo los indices que poseen los mejores planos
    indiceX = this->indiceMejorPlanoX;
    indiceY = this->indiceMejorPlanoY;
    indiceZ = this->indiceMejorPlanoZ;
}

SimpleMesh::Mesh* Mesh::replicarGeometriaSimetrica(Plano3D pl, int axis, float coord){
    SimpleMesh::Vertex point(0.0, 0.0, 0.0);

    if(axis == 1){ //Eje X
        point.setX(coord);
    }else if(axis==2){ // Eje Y
        point.setY(coord);
    }else if(axis==3){ // Eje Z
        point.setZ(coord);
    }

    //Convert vertices to homogeneous representation
    MatrixXd hom_vertex(4, num_vertices);
    for(unsigned int i = 0; i < num_vertices; i++){
       hom_vertex(0,i) = vertices[i].x();
       hom_vertex(1,i) = vertices[i].y();
       hom_vertex(2,i) = vertices[i].z();
       hom_vertex(3,i) = 1.0;
    }


    Matrix4d T;
    T(0,0) = 1.0;   T(0,1) = 0.0; T(0,2) = 0.0; T(0,3) = -point.x();
    T(1,0) = 0.0;   T(1,1) = 1.0; T(1,2) = 0.0; T(1,3) = -point.y();
    T(2,0) = 0.0;   T(2,1) = 0.0; T(2,2) = 1.0; T(2,3) = -point.z();
    T(3,0) = 0.0;   T(3,1) = 0.0; T(3,2) = 0.0; T(3,3) = 1.0;

    Matrix4d Tinv;
    Tinv(0,0) = 1.0;   Tinv(0,1) = 0.0; Tinv(0,2) = 0.0; Tinv(0,3) = point.x();
    Tinv(1,0) = 0.0;   Tinv(1,1) = 1.0; Tinv(1,2) = 0.0; Tinv(1,3) = point.y();
    Tinv(2,0) = 0.0;   Tinv(2,1) = 0.0; Tinv(2,2) = 1.0; Tinv(2,3) = point.z();
    Tinv(3,0) = 0.0;   Tinv(3,1) = 0.0; Tinv(3,2) = 0.0; Tinv(3,3) = 1.0;

    Vector3d M(pl.getA(), pl.getB(), pl.getC());
    Vector3d N(0.0, 0.0, 1.0);

    Vector3d axisV = M.cross(N)/M.cross(N).norm();
    float costheta = M.dot(N)/(M.norm()*N.norm());
    float x = axisV(0), y=axisV(1), z=axisV(2);

    float c = costheta;
    float s = sqrt(1 - c*c);
    float tt = 1 - c;

    //Rotation matrix
    Matrix4d Q;
    Q(0,0) = tt*x*x + c;       Q(0,1) = tt*x*y - s*z;       Q(0,2) = tt*x*z + s*y;       Q(0,3) = 0.0;
    Q(1,0) = tt*x*y + s*z;     Q(1,1) = tt*y*y + c;         Q(1,2) = tt*y*z - s*x;       Q(1,3) = 0.0;
    Q(2,0) = tt*x*z - s*y;     Q(2,1) = tt*y*z + s*x;       Q(2,2) = tt*z*z + c;         Q(2,3) = 0.0;
    Q(3,0) = 0.0;              Q(3,1) = 0.0;                Q(3,2) = 0.0;                Q(3,3) = 1.0;

    s = -s;

    //Inverse rotation matrix
    Matrix4d Qinv;
    Qinv(0,0) = tt*x*x + c;       Qinv(0,1) = tt*x*y - s*z;       Qinv(0,2) = tt*x*z + s*y;       Qinv(0,3) = 0.0;
    Qinv(1,0) = tt*x*y + s*z;     Qinv(1,1) = tt*y*y + c;         Qinv(1,2) = tt*y*z - s*x;       Qinv(1,3) = 0.0;
    Qinv(2,0) = tt*x*z - s*y;     Qinv(2,1) = tt*y*z + s*x;       Qinv(2,2) = tt*z*z + c;         Qinv(2,3) = 0.0;
    Qinv(3,0) = 0.0;              Qinv(3,1) = 0.0;                Qinv(3,2) = 0.0;                Qinv(3,3) = 1.0;

    //Create transformation matrix (reflection)
    Matrix4d Ref;
    Ref(0,0) = 1.0;     Ref(0,1) = 0.0;     Ref(0,2) = 0.0;     Ref(0,3) = 0.0;
    Ref(1,0) = 0.0;     Ref(1,1) = 1.0;     Ref(1,2) = 0.0;     Ref(1,3) = 0.0;
    Ref(2,0) = 0.0;     Ref(2,1) = 0.0;     Ref(2,2) = -1.0;    Ref(2,3) = 0.0;
    Ref(3,0) = 0.0;     Ref(3,1) = 0.0;     Ref(3,2) = 0.0;     Ref(3,3) = 1.0;

    //Complete transformation for normalized object
    Matrix4d TransfBase = Tinv*Qinv*Ref*Q*T;
    MatrixXd vertexSource = TransfBase * hom_vertex;    //modelo transformado escalado


    SimpleMesh::Mesh* output = new SimpleMesh::Mesh();
    output->set_number_vertices(num_vertices);
    output->set_number_triangles(num_triangles);

    for(unsigned int i = 0; i < num_vertices; i++){
        output->add_vertex(i, vertexSource(0,i) - 5, vertexSource(1,i) +1, vertexSource(2,i));
    }

    for(unsigned int i = 0; i < num_triangles; i++){
        vector<unsigned int> vert = triangles[i].get_vertices();
        output->add_triangle(i, vert[0], vert[2], vert[1]);
    }

    output->compute_normals();
    return output;

}

}//namespace SimpleMesh
