#ifndef OBJECTCONTAINER_H
#define OBJECTCONTAINER_H

#include <QVector3D>
#include <QOpenGLShaderProgram>
#include "SimpleMesh/mesh.h"
#include "bufferedobject3d.h"

//This class stores the objects in the application. This object contains two collections, the original objects and
//the corresponding buffered objects
class ObjectContainer
{

    vector<SymArchData::BufferedObject3D*> m_bufferedObjects;
    vector<bool> m_visibility;
    vector<QString> m_names;

    int selectedObject;


    float scale;
    QVector3D center;

public:
    vector<SimpleMesh::Mesh*> m_objects;
    float bbox[2][3];
    float bbox_modificado[2][3];
    ObjectContainer();
    ~ObjectContainer();
    void addObject(SimpleMesh::Mesh* new_mesh, SymArchData::BufferedObject3D* new_buffered_object, QString name);
    void replaceObject(SimpleMesh::Mesh *new_mesh, SymArchData::BufferedObject3D *new_buffered_object, QString name, int indice);
    //void drawObjects(QOpenGLShaderProgram* program);

    void drawObjects(QOpenGLShaderProgram* program, int indiceCara, int posicionCara, bool isMoverCara, bool isResizeCara, unsigned char* mapcolor_X,unsigned char* mapcolor_Y,unsigned char* mapcolor_Z);

    float getScale(){return scale;}
    QVector3D getCenter(){return center;}
    void removeObject(int index);
    void recomputeScaleAndCenter();
    void setVisibility(int index, bool vis){m_visibility[index] = vis;}
    bool getVisibility(int index) {return m_visibility[index]; }
    QString getName(int index){ return m_names[index];}

    void drawColorBar(QOpenGLShaderProgram* program, int indiceCara, int posicionCara, bool isMoverCara, bool isResizeCara, unsigned char* mapcolor_X,unsigned char* mapcolor_Y,unsigned char* mapcolor_Z);
    void drawObjectsAndColorBar(QOpenGLShaderProgram* programObject, QOpenGLShaderProgram* programColorBar);
    void drawBoundingBox(QOpenGLShaderProgram* program);
    void clearData();
    int getNumberObjects(){return m_objects.size();}
    SimpleMesh::Mesh* getObject(int index){return m_objects[index];}
    SymArchData::BufferedObject3D* getBufferedObject(int index){return m_bufferedObjects[index];}

    void select(int ind){selectedObject = ind;}
    int selection(){return selectedObject;}

    unsigned char* mapcolor_X = new unsigned char[64 * 3 + 1];
    unsigned char* mapcolor_Y = new unsigned char[64 * 3 + 1];
    unsigned char* mapcolor_Z = new unsigned char[64 * 3 + 1];

    unsigned char* mapcolor_X_Edit = new unsigned char[64 * 3 + 1];
    unsigned char* mapcolor_Y_Edit = new unsigned char[64 * 3 + 1];
    unsigned char* mapcolor_Z_Edit = new unsigned char[64 * 3 + 1];

    SimpleMesh::Plano3D mejorPlanoX;
    SimpleMesh::Plano3D mejorPlanoY;
    SimpleMesh::Plano3D mejorPlanoZ;

    int indiceMejorPlanoX;
    int indiceMejorPlanoY;
    int indiceMejorPlanoZ;

    int indiceActualPlanoX;
    int indiceActualPlanoY;
    int indiceActualPlanoZ;


    vector<SimpleMesh::Plano3D> listadoPlanosX;
    vector<SimpleMesh::Plano3D> listadoPlanosY;
    vector<SimpleMesh::Plano3D> listadoPlanosZ;

    vector<float> datosX;
    vector<float> datosY;
    vector<float> datosZ;
};

#endif // OBJECTCONTAINER_H
