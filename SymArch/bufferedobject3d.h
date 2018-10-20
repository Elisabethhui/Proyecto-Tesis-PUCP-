#ifndef BUFFEREDOBJECT3D_H
#define BUFFEREDOBJECT3D_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include "SimpleMesh/mesh.h"
#include <QOpenGLFunctions_4_1_Core>

/**
 * The class BufferedObject3D stores complementary information for a mesh
 *
 * Basically, it stores the information related to linear buffers and opengl stuff
 */
#include <QOpenGLFunctions>

QT_FORWARD_DECLARE_CLASS(GLWidget)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

namespace SymArchData{

class BufferedObject3D : protected QOpenGLFunctions
{
private:
    float*          m_vertices;
    float*          m_normals;
    float*          m_colors;
    unsigned int*   m_indices;

    QOpenGLBuffer   m_vertexBuffer;
    QOpenGLBuffer   m_normalBuffer;
    QOpenGLBuffer   m_colorBuffer;
    QOpenGLBuffer   m_indexBuffer;

    QOpenGLVertexArrayObject m_vao;
    int num_vertices;
    int num_triangles;
    GLWidget* glArea;
    QVector3D color;

    int index;

public:
    BufferedObject3D();
    ~BufferedObject3D();

    int getVertices();
    int getTriangles();

    //This function transforms the data from a storage Mesh into a buffered object
    //It also initializes the VAO and VBO's
    void createBuffersFromObject(SimpleMesh::Mesh* mesh);
    //void drawBufferedObject(QOpenGLShaderProgram* program,float bbox[][3], int indice);

    void drawBufferedObject(QOpenGLShaderProgram* program,
                            float bbox[][3],
                            int indice,
                            int indiceCara,
                            int posicionCara, bool isMoverCara, bool isResizeCara,
                            unsigned char* mapcolor_X,
                            unsigned char* mapcolor_Y,
                            unsigned char* mapcolor_Z);

    void drawBufferedBoundingObject(QOpenGLShaderProgram* program, float bbox[][3],int vertexLocation);
    void drawBufferedColorBarAndPaint(QOpenGLShaderProgram* program, float bbox[][3],int vertexLocation);
    void drawWall(QOpenGLShaderProgram* program, float bbox[][3], unsigned char* mapcolor, int);
    void drawWallInverse(QOpenGLShaderProgram* program, float bbox[][3], unsigned char* mapcolor, int);
    void setGLWidget(GLWidget* glWidget){glArea = glWidget;}
    void setColor(QVector3D v){color.setX(v.x()); color.setY(v.y()); color.setZ(v.z());}

    void drawBufferedColorBar(QOpenGLShaderProgram* program, float bbox[][3], int indice,unsigned char* mapcolor_X, unsigned char* mapcolor_Y, unsigned char* mapcolor_Z);

    void setIndex(int ind){index = ind;}
    int id(){return index;}

    unsigned char pixel_data_x[64 * 3 + 1]={
            159,	255,	96,
            175,	255,	80,
            191,	255,	64,
            207,	255,	48,
            223,	255,	32,
            239,	255,	16,
            255,	255,	0,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            215,	42,	0,
            210,	40,	0,
            215,	42,	0,
            210,	40,	0,
            207,	16,	0,
            215,	42,	0,
            210,	40,	0,
            207,	16,	0,
            215,	42,	0,
            210,	40,	0,
            207,	16,	0,
            215,	42,	0,
            210,	40,	0,
            207,	16,	0,
            215,	42,	0,
            210,	40,	0,
            207,	16,	0,
            215,	42,	0,
            210,	40,	0,
            207,	16,	0,
            215,	42,	0,
            210,	40,	0,
            207,	16,	0,
            203,	16,	0,
            199,	16,	0,
            196,	16,	0,
            192,	0,	0,
            188,	0,	0,
            184,	0,	0,
            180,	0,	0,
            176,	0,	0,
            172,	0,	0,
            168,	0,	0,
            164,	0,	0,
            160,	0,	0,
            156,	0,	0,
            152,	0,	0,
            148,	0,	0,
            144,	0,	0,
            140,	0,	0,
            137,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0
        };


    unsigned char pixel_data_y[64 * 3 + 1]={
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            176,	16,	0,
            174,	16,	0,
            170,	0,	0,
            168,	0,	0,
            176,	16,	0,
            174,	16,	0,
            170,	0,	0,
            168,	0,	0,
            176,	16,	0,
            174,	16,	0,
            170,	0,	0,
            168,	0,	0,
            176,	16,	0,
            174,	16,	0,
            170,	0,	0,
            168,	0,	0,
            176,	16,	0,
            174,	16,	0,
            170,	0,	0,
            168,	0,	0,
            164,	0,	0,
            162,	0,	0,
            160,	0,	0,
            158,	0,	0,
            155,	0,	0,
            152,	0,	0,
            150,	0,	0,
            148,	0,	0,
            146,	0,	0,
            144,	0,	0,
            140,	0,	0,
            136,	0,	0,
            134,	0,	0,
            132,	0,	0,
            130,	0,	0,
            128,	0,	0
        };

    unsigned char pixel_data_z[64 * 3 + 1]={
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            130,	0,	0,
            128,	0,	0,
            134,	0,	0,
            130,	0,	0,
            128,	0,	0,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            112,	255,	143,
            159,	0,	0,
            143,	0,	0,
            128,	0,	0
        };

    unsigned char tapas[4 * 3]={
        0,	0,	0,
        0,	0,	0,
        0,	0,	0,
        0,	0,  0,
    };
};

}
#endif // BUFFEREDOBJECT3D_H
