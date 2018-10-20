#ifndef MESH_H
#define MESH_H

#include "SimpleMesh/simplemesh_global.h"
#include "SimpleMesh/vertex.h"
#include "SimpleMesh/triangle.h"

namespace SimpleMesh{

class SIMPLEMESH_API Plano3D{
public:
    float a,b,c,d;
    float getA(){return a;}
    float getB(){return b;}
    float getC(){return c;}
    float getD(){return d;}
    Plano3D():a(0.0), b(0.0),c(0.0), d(0.0){}
};

class SIMPLEMESH_API Mesh
{
protected:
    Vertex*         vertices;
    Triangle*       triangles;
    unsigned int    num_vertices;
    unsigned int    num_triangles;

    float xmin, xmax, ymin, ymax, zmin, zmax;
    float diag;

    bool flag_normals, flag_color;

public:
    Mesh();
    ~Mesh();

    Vertex*     get_vertices() {return vertices;}
    Triangle*   get_triangles() {return triangles;}

    unsigned int get_number_vertices() { return num_vertices; }
    unsigned int get_number_triangles() { return num_triangles;}

    void add_vertex(unsigned int pos, float x, float y, float z);
    void add_triangle(unsigned int pos, unsigned int p1, unsigned int p2, unsigned int p3);
    void add_vertex(unsigned int pos, Vertex v);
    void set_vertex(unsigned int pos, float x, float y, float z);
    void add_normal(unsigned int pos, float nx, float ny, float nz);
    void add_color(unsigned int pos, float r, float g, float b);

    void set_number_vertices(unsigned int nv);
    void set_number_triangles(unsigned int nt);
    void compute_normals();

    float get_diagonal();
    float get_area();

    bool has_normals(){return flag_normals;}
    bool has_color(){return flag_color;}

    void enable_normals(){flag_normals = true;}
    void enable_color(){flag_color = true;}
    float get_xmin();
    float get_ymin();
    float get_zmin();
    float get_xmax();
    float get_ymax();
    float get_zmax();
    unsigned char* GetSimetriaX();
    unsigned char* GetSimetriaY();
    unsigned char* GetSimetriaZ();
    float * GetSimetriaEstadistica();

    vector<float> GetSimetriaXValues();
    vector<float> GetSimetriaYValues();
    vector<float> GetSimetriaZValues();
    void ComputeBarsSymmetry(unsigned char* mapcolorX,
                             unsigned char* mapcolorY,
                             unsigned char* mapcolorZ,
                             int &indiceX, int &indiceY, int &indiceZ);
    Plano3D mejorPlanoX;
    Plano3D mejorPlanoY;
    Plano3D mejorPlanoZ;

    int indiceMejorPlanoX;
    int indiceMejorPlanoY;
    int indiceMejorPlanoZ;

    //Aqui el listado de planos
    vector<Plano3D> planosX;
    vector<Plano3D> planosY;
    vector<Plano3D> planosZ;

    SimpleMesh::Mesh* replicarGeometriaSimetrica(Plano3D pl, int axis, float coord); //axis = 1 (X), axis = 2 (Y), axis = 3 (Z)

    float computar_votos(Plano3D pl, float* K1, float* K2);
};

} //namespace SimpleMesh

#endif // MESH_H
