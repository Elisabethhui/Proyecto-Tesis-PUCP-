#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

#include "SimpleMesh/mesh.h"
#include "objectcontainer.h"
#include "ArcBall.h"
#include "analysisbar.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QOpenGLShaderProgram* getProgram(){return m_programs[MAIN_OBJECT_PROGRAM];}
    QVector3D getColorPalette(int index){return palette[index];}
    void keyPressEvent(QKeyEvent *e);
    void setObjectContainer(ObjectContainer* oc){ m_container = oc;}
    void setValuePress(bool valor);
    static bool valuePressEdit;
    static bool selectPlanes;
    static bool rebuild;

public Q_SLOTS:

    void cleanup();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    //void keyPressEvent(QKeyEvent *e);

private:
    bool m_core;

    //Shader attribute locations
    GLint attribute_v_coord;
    GLint attribute_v_normal;
    GLint uniform_m;
    GLint uniform_v;
    GLint uniform_p;
    GLint uniform_m_3x3_inv_transp;
    GLint uniform_v_inv;
    //Para el bounding box
    GLint uniform_m_bounding_box;
    GLint uniform_v_bounding_box;
    GLint uniform_p_bounding_box;
    GLint uniform_m_3x3_inv_transp_bounding_box;
    GLint uniform_v_inv_bounding_box;
    GLint uniform_mvp;

    enum{
        MAIN_OBJECT_PROGRAM,
        BOUNDING_BOX_PROGRAM,
        COLORBAR_PROGRAM,
        NUM_PROGRAMS
    };

    QOpenGLShaderProgram* m_programs[3];
    QMatrix4x4 m_proj;
    QMatrix4x4 m_cam;
    QMatrix4x4 m_world;
    bool m_transparent;

    float scale;
    QVector3D center;

    float delta_scale;

    int screen_width;
    int screen_height;

    //Arcball
    QMatrix4x4 Transform;
    QMatrix3x3 LastRot;
    QMatrix3x3 ThisRot;

    ArcBallT ArcBall;
    QPointF MousePt;
    bool isClicked;

    //Bounding Box: we only store the vertices, the connectivity is hard-coded in the rendering
    SimpleMesh::Vertex bbox[8];
    SimpleMesh::Vertex bbox_Modify[8];
    int indiceCara;
    int posicionCara;
    bool isMoverCara;
    bool isResizeCara;
    int indiceBarras; //Es barra X (0) o Y (1) o Z(2)

    bool isMoverAnillo;

    //Bar of the objetc
    AnalysisBar analysisBar;

    QVector3D palette[8];
    ObjectContainer* m_container;

};

#endif // GLWIDGET_H
