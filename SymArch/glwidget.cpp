 #include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QtMath>
#include <math.h>
#include <iostream>
#include <QKeyEvent>

#include "SimpleMesh/io/offreader.h"
#include "SimpleMesh/io/offwriter.h"
#include "analysisbar.h"
#include "mainwindow.h"

using namespace std;
GLWidget::GLWidget(QWidget* parent)
    :QOpenGLWidget(parent),
      ArcBall(400.0f, 400.0f), analysisBar(50,50,100.0f)

{
    cout << "GLWidget::GLWidget" << endl;
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);

    Transform.setToIdentity();
    LastRot.setToIdentity();
    ThisRot.setToIdentity();
    isClicked = false;
    delta_scale = 0.0;

    m_programs[MAIN_OBJECT_PROGRAM] = NULL;
    m_programs[BOUNDING_BOX_PROGRAM] = NULL;
    m_programs[COLORBAR_PROGRAM] = NULL;

    //Seteando indices
    this->posicionCara = -1;
    this->indiceCara = -1;
    this->indiceBarras =-1;
    //this->isMoverCara = false;
    this->isMoverCara = true;
    this->isResizeCara = false;
    this->isMoverAnillo = false;


    //Inicializar la paleta de colores
    palette[0].setX(0.86);  palette[0].setY(0.86);  palette[0].setZ(0.86); //Gris claro, color por defecto
    palette[1].setX(1.00);  palette[1].setY(1.00);  palette[1].setZ(0.24); //Amarillo
    palette[2].setX(0.84);  palette[2].setY(1.00);  palette[2].setZ(0.24); //Verde limon
    palette[3].setX(0.80);  palette[3].setY(0.90);  palette[3].setZ(1.00); //Celeste
    palette[4].setX(0.79);  palette[4].setY(0.58);  palette[4].setZ(0.89); //Morado
    palette[5].setX(0.00);  palette[5].setY(0.60);  palette[5].setZ(1.00); //Azul
    palette[6].setX(1.00);  palette[6].setY(0.80);  palette[6].setZ(0.40); //Maiz
    palette[7].setX(0.60);  palette[7].setY(0.60);  palette[7].setZ(0.60); //Gris oscuro
    valuePressEdit = false;
    selectPlanes = false;
    rebuild = false;
}

bool GLWidget::valuePressEdit;

bool GLWidget::selectPlanes;

bool GLWidget::rebuild;

void GLWidget::setValuePress(bool valor){
    valuePressEdit=valor;
}

GLWidget::~GLWidget(){
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle){
    while(angle < 0)
        angle += 360*16;
    while(angle > 360*16)
        angle -=360*16;
}

void GLWidget::cleanup(){
    makeCurrent();
    for(int i = 0; i < NUM_PROGRAMS; i++)
        if(m_programs[i]!=NULL)
            delete m_programs[i];
    doneCurrent();
}

void GLWidget::initializeGL(){
    bool ok = true;
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(1, 1, 1, m_transparent ? 0 : 1);

    //Set the shaders for main object
    m_programs[MAIN_OBJECT_PROGRAM] = new QOpenGLShaderProgram;
    ok = m_programs[MAIN_OBJECT_PROGRAM]->addShaderFromSourceFile(QOpenGLShader::Vertex, "phong-shading.v.glsl");
    if(!ok){
        cout << "Problem compiling Vertex Shader" << endl;
        cout << m_programs[MAIN_OBJECT_PROGRAM]->log().toStdString() << endl;
        qApp->exit(-1);
    }

    ok = m_programs[MAIN_OBJECT_PROGRAM]->addShaderFromSourceFile(QOpenGLShader::Fragment, "phong-shading.f.glsl");
    if(!ok){
        cout << "Problem compiling Fragment Shader"<< endl;
        cout << m_programs[MAIN_OBJECT_PROGRAM]->log().toStdString() << endl;
        qApp->exit(-1);
    }

    m_programs[MAIN_OBJECT_PROGRAM]->link();

    //Color Bar
    m_programs[COLORBAR_PROGRAM] = new QOpenGLShaderProgram;
    ok = m_programs[COLORBAR_PROGRAM]->addShaderFromSourceFile(QOpenGLShader::Vertex, "texture.v.glsl");
    if(!ok){
        cout << "Problem compiling Fragment Shader"<< endl;
        cout << m_programs[COLORBAR_PROGRAM]->log().toStdString() << endl;
        qApp->exit(-1);
    }

    ok = m_programs[COLORBAR_PROGRAM]->addShaderFromSourceFile(QOpenGLShader::Fragment, "texture.f.glsl");
    if(!ok){
        cout << "Problem compiling Fragment Shader"<< endl;
        cout << m_programs[COLORBAR_PROGRAM]->log().toStdString() << endl;
        qApp->exit(-1);
    }

    m_programs[COLORBAR_PROGRAM]->link();

    //Set the shaders for bounding box
    m_programs[BOUNDING_BOX_PROGRAM] = new QOpenGLShaderProgram;
    ok = m_programs[BOUNDING_BOX_PROGRAM]->addShaderFromSourceFile(QOpenGLShader::Vertex, "bounding-shading.v.glsl");
    if(!ok){
        cout << "Problem compiling Vertex Shader" << endl;
        cout << m_programs[BOUNDING_BOX_PROGRAM]->log().toStdString() << endl;
        qApp->exit(-1);
    }

    ok = m_programs[BOUNDING_BOX_PROGRAM]->addShaderFromSourceFile(QOpenGLShader::Fragment, "bounding-shading.f.glsl");
    if(!ok){
        cout << "Problem compiling Fragment Shader" << endl;
        cout << m_programs[BOUNDING_BOX_PROGRAM]->log().toStdString() << endl;
        qApp->exit(-1);
    }

    m_programs[BOUNDING_BOX_PROGRAM]->link();

    m_programs[MAIN_OBJECT_PROGRAM]->bind();
        uniform_m = m_programs[MAIN_OBJECT_PROGRAM]->uniformLocation("m");
        uniform_v = m_programs[MAIN_OBJECT_PROGRAM]->uniformLocation("v");
        uniform_p = m_programs[MAIN_OBJECT_PROGRAM]->uniformLocation("p");
        uniform_m_3x3_inv_transp = m_programs[MAIN_OBJECT_PROGRAM]->uniformLocation("m_3x3_inv_transp");
        uniform_v_inv = m_programs[MAIN_OBJECT_PROGRAM]->uniformLocation("v_inv");
    m_programs[MAIN_OBJECT_PROGRAM]->release();

    m_programs[COLORBAR_PROGRAM]->bind();
        uniform_mvp = m_programs[COLORBAR_PROGRAM]->uniformLocation("mvp");
    m_programs[COLORBAR_PROGRAM]->release();

    m_cam.setToIdentity();
    m_cam.translate(0, 0, -1);
    m_cam.lookAt(QVector3D(0.0, 0.0, 4.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));

    cout << "OpenGL initialized OK!" << endl;

}

void GLWidget::keyPressEvent(QKeyEvent *e){
    cout << "BOTON " << char(e->key())<<endl;
    cout << "VALUE PRESS " << this->valuePressEdit << endl;
    //Escoger las caras
    if(valuePressEdit){
        if(this->isMoverCara){
            if(e->key() == Qt::Key_A){
                this->indiceCara = 0;
            }else if(e->key() == Qt::Key_D){
                this->indiceCara = 1;
            }else if(e->key() == Qt::Key_S){
                this->indiceCara = 2;
            }else if(e->key() == Qt::Key_W){
                this->indiceCara = 3;
            }else if(e->key() == Qt::Key_R){
                this->indiceCara = 4;
            }else if(e->key() == Qt::Key_F){
                this->indiceCara = 5;
            }else if(e->key() == Qt::Key_Return){
                this->posicionCara = -1;
                this->isMoverCara = false;
                this->isResizeCara = true;
            }else if(e->key() == Qt::Key_Escape){
                int value = QMessageBox::question(this, tr("Edit Bounding Box"),
                                   tr("Do you want to exit of the edition tool?"), QMessageBox::Yes|QMessageBox::No);

                if (value == QMessageBox::Yes) {
                    this->posicionCara = -1;
                    this->indiceCara = -1;
                    this->isResizeCara = false;
                    this->isMoverCara = true;
                    this->valuePressEdit = false;
                    valuePressEdit = false;
                } else {
                    valuePressEdit = true;
                }
            }
        }
        //Escoger si se desea alejar la cara del objeto o no
        if(this->isResizeCara){
            if(e->key() == Qt::Key_Plus){
                this->posicionCara = 1;
            }else if(e->key() == Qt::Key_Minus){
                this->posicionCara = 0;
            }else if(e->key() == Qt::Key_Escape){
                int value = QMessageBox::question(this, tr("Edit Bounding Box"),
                                   tr("Do you want to exit of the edition tool?"), QMessageBox::Yes|QMessageBox::No);

                if (value == QMessageBox::Yes) {
                    this->posicionCara = -1;
                    this->indiceCara = -1;
                    this->isResizeCara = false;
                    this->isMoverCara = true;
                    this->valuePressEdit = false;
                    valuePressEdit = false;
                } else {
                    valuePressEdit = true;
                }
            }
        }

        if(this->posicionCara!=-1){//Es por que si se selecciona el + o -
            if(this->indiceCara == 0){ //Izquierda
                if(this->posicionCara == 1){
                    m_container->bbox_modificado[0][0] = m_container->bbox_modificado[0][0] - 1;
                }else{
                    m_container->bbox_modificado[0][0] = m_container->bbox_modificado[0][0] + 1;
                }
            }else if(this->indiceCara == 1){ //Derecha
                if(this->posicionCara == 1){
                    m_container->bbox_modificado[1][0] = m_container->bbox_modificado[1][0] + 1;
                }else{
                    m_container->bbox_modificado[1][0] = m_container->bbox_modificado[1][0] - 1;
                }
            }else if(this->indiceCara == 3){ //Arriba
                if(this->posicionCara == 1){
                    m_container->bbox_modificado[1][1] = m_container->bbox_modificado[1][1] + 1;
                }else{
                    m_container->bbox_modificado[1][1] = m_container->bbox_modificado[1][1] - 1;
                }
            }else if(this->indiceCara == 2){ //Abajo
                if(this->posicionCara == 1){
                    m_container->bbox_modificado[0][1] = m_container->bbox_modificado[0][1] - 1;
                }else{
                    m_container->bbox_modificado[0][1] = m_container->bbox_modificado[0][1] + 1;
                }
            }else if(this->indiceCara == 4){ //Frente
                if(this->posicionCara == 1){
                    m_container->bbox_modificado[1][2] = m_container->bbox_modificado[1][2] + 1;
                }else{
                    m_container->bbox_modificado[1][2] = m_container->bbox_modificado[1][2] - 1;
                }
            }else if(this->indiceCara == 5){ //Atras
                if(this->posicionCara == 1){
                    m_container->bbox_modificado[0][2] = m_container->bbox_modificado[0][2] - 1;
                }else{
                    m_container->bbox_modificado[0][2] = m_container->bbox_modificado[0][2] + 1;
                }
            }
        }

        setFocus();
        update();
    }else{
        cout << "Los planos" << endl;
        if(this->selectPlanes){//Escoger los planos
            if(e->key() == Qt::Key_X){
                this->indiceBarras = 0;
            }else if(e->key() == Qt::Key_Y){
                this->indiceBarras = 1;
            }else if(e->key() == Qt::Key_Z){
                this->indiceBarras = 2;
            }else if(e->key() == Qt::Key_Return){
                this->selectPlanes = false;
                this->isMoverAnillo = true;
                cout << "ENTRO" << endl;
            }else if(e->key() == Qt::Key_Escape){
                int value = QMessageBox::question(this, tr("Select palnes"),
                                   tr("Do you want to exit of the selection of planes?"), QMessageBox::Yes|QMessageBox::No);
                if (value == QMessageBox::Yes) {
                    this->selectPlanes = false;
                    this->indiceBarras = -1;
                    this->isMoverAnillo = false;
                } else {
                    this->selectPlanes = true;
                }
            }
        }

        if(this->isMoverAnillo){
            if(indiceBarras == 0){ //X
                if(e->key() == Qt::Key_Plus){
                    if(m_container->indiceActualPlanoX < 63){ //Limite superior
                       m_container->indiceActualPlanoX++;
                       m_container->mapcolor_X_Edit[m_container->indiceActualPlanoX*3] = 0;
                       m_container->mapcolor_X_Edit[m_container->indiceActualPlanoX*3 + 1] = 0;
                       m_container->mapcolor_X_Edit[m_container->indiceActualPlanoX*3 + 2] = 0;

                       m_container->mapcolor_X_Edit[(m_container->indiceActualPlanoX - 1)*3] = m_container->mapcolor_X[(m_container->indiceActualPlanoX - 1)*3];
                       m_container->mapcolor_X_Edit[(m_container->indiceActualPlanoX - 1)*3 + 1] = m_container->mapcolor_X[(m_container->indiceActualPlanoX - 1)*3 + 1];
                       m_container->mapcolor_X_Edit[(m_container->indiceActualPlanoX - 1)*3 + 2] = m_container->mapcolor_X[(m_container->indiceActualPlanoX - 1)*3 + 2];
                    }
                }else if(e->key() == Qt::Key_Minus){
                    if(m_container->indiceActualPlanoX > 0){
                        m_container->indiceActualPlanoX--;

                        m_container->mapcolor_X_Edit[m_container->indiceActualPlanoX*3] = 0;
                        m_container->mapcolor_X_Edit[m_container->indiceActualPlanoX*3 + 1] = 0;
                        m_container->mapcolor_X_Edit[m_container->indiceActualPlanoX*3 + 2] = 0;

                        m_container->mapcolor_X_Edit[(m_container->indiceActualPlanoX + 1)*3] = m_container->mapcolor_X[(m_container->indiceActualPlanoX + 1)*3];
                        m_container->mapcolor_X_Edit[(m_container->indiceActualPlanoX + 1)*3 + 1] = m_container->mapcolor_X[(m_container->indiceActualPlanoX + 1)*3 + 1];
                        m_container->mapcolor_X_Edit[(m_container->indiceActualPlanoX + 1)*3 + 2] = m_container->mapcolor_X[(m_container->indiceActualPlanoX + 1)*3 + 2];
                    }
                }
            }else if(indiceBarras == 1){ //Y
                if(e->key() == Qt::Key_Plus){
                    if(m_container->indiceActualPlanoY < 63){ //Limite superior
                       m_container->indiceActualPlanoY++;

                       m_container->mapcolor_Y_Edit[m_container->indiceActualPlanoY*3] = 0;
                       m_container->mapcolor_Y_Edit[m_container->indiceActualPlanoY*3 + 1] = 0;
                       m_container->mapcolor_Y_Edit[m_container->indiceActualPlanoY*3 + 2] = 0;

                       m_container->mapcolor_Y_Edit[(m_container->indiceActualPlanoY - 1)*3] = m_container->mapcolor_Y[(m_container->indiceActualPlanoY - 1)*3];
                       m_container->mapcolor_Y_Edit[(m_container->indiceActualPlanoY - 1)*3 + 1] = m_container->mapcolor_Y[(m_container->indiceActualPlanoY - 1)*3 + 1];
                       m_container->mapcolor_Y_Edit[(m_container->indiceActualPlanoY - 1)*3 + 2] = m_container->mapcolor_Y[(m_container->indiceActualPlanoY - 1)*3 + 2];
                    }
                }else if(e->key() == Qt::Key_Minus){
                    if(m_container->indiceActualPlanoY > 0){
                        m_container->indiceActualPlanoY--;

                        m_container->mapcolor_Y_Edit[m_container->indiceActualPlanoY*3] = 0;
                        m_container->mapcolor_Y_Edit[m_container->indiceActualPlanoY*3 + 1] = 0;
                        m_container->mapcolor_Y_Edit[m_container->indiceActualPlanoY*3 + 2] = 0;

                        m_container->mapcolor_Y_Edit[(m_container->indiceActualPlanoY + 1)*3] = m_container->mapcolor_Y[(m_container->indiceActualPlanoY + 1)*3];
                        m_container->mapcolor_Y_Edit[(m_container->indiceActualPlanoY + 1)*3 + 1] = m_container->mapcolor_Y[(m_container->indiceActualPlanoY + 1)*3 + 1];
                        m_container->mapcolor_Y_Edit[(m_container->indiceActualPlanoY + 1)*3 + 2] = m_container->mapcolor_Y[(m_container->indiceActualPlanoY + 1)*3 + 2];
                    }
                }
            }else if(indiceBarras == 2){
                if(e->key() == Qt::Key_Plus){
                    if(m_container->indiceActualPlanoZ < 63){ //Limite superior
                       m_container->indiceActualPlanoZ++;

                       m_container->mapcolor_Z_Edit[m_container->indiceActualPlanoZ*3] = 0;
                       m_container->mapcolor_Z_Edit[m_container->indiceActualPlanoZ*3 + 1] = 0;
                       m_container->mapcolor_Z_Edit[m_container->indiceActualPlanoZ*3 + 2] = 0;

                       m_container->mapcolor_Z_Edit[(m_container->indiceActualPlanoZ - 1)*3] = m_container->mapcolor_Z[(m_container->indiceActualPlanoZ - 1)*3];
                       m_container->mapcolor_Z_Edit[(m_container->indiceActualPlanoZ - 1)*3 + 1] = m_container->mapcolor_Z[(m_container->indiceActualPlanoZ - 1)*3 + 1];
                       m_container->mapcolor_Z_Edit[(m_container->indiceActualPlanoZ - 1)*3 + 2] = m_container->mapcolor_Z[(m_container->indiceActualPlanoZ - 1)*3 + 2];
                    }
                }else if(e->key() == Qt::Key_Minus){
                    if(m_container->indiceActualPlanoZ > 0){
                        m_container->indiceActualPlanoZ--;

                        m_container->mapcolor_Z_Edit[m_container->indiceActualPlanoZ*3] = 0;
                        m_container->mapcolor_Z_Edit[m_container->indiceActualPlanoZ*3 + 1] = 0;
                        m_container->mapcolor_Z_Edit[m_container->indiceActualPlanoZ*3 + 2] = 0;

                        m_container->mapcolor_Z_Edit[(m_container->indiceActualPlanoZ + 1)*3] = m_container->mapcolor_Z[(m_container->indiceActualPlanoZ + 1)*3];
                        m_container->mapcolor_Z_Edit[(m_container->indiceActualPlanoZ + 1)*3 + 1] = m_container->mapcolor_Z[(m_container->indiceActualPlanoZ + 1)*3 + 1];
                        m_container->mapcolor_Z_Edit[(m_container->indiceActualPlanoZ + 1)*3 + 2] = m_container->mapcolor_Z[(m_container->indiceActualPlanoZ + 1)*3 + 2];
                    }
                }
            }
            if(e->key() == Qt::Key_Escape){//Salir del modo
                int value = QMessageBox::question(this, tr("Select palnes"),
                                   tr("Do you want to exit of the selection of planes?"), QMessageBox::Yes|QMessageBox::No);
                if (value == QMessageBox::Yes) {
                    this->selectPlanes = false;
                    this->indiceBarras = -1;
                    this->isMoverAnillo = false;
                } else {
                    this->isMoverAnillo = true;
                }
            }
        }

        setFocus();
        update();
    }
}

void GLWidget::paintGL(){
    cout << "Enter paintGL" << endl;
    scale = 1.5*m_container->getScale();
    center = m_container->getCenter();

    QMatrix4x4 translation;
    translation.setToIdentity();
    translation.translate(-center);

    QMatrix4x4 scal;
    scal.setToIdentity();
    scal.scale(scale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_programs[MAIN_OBJECT_PROGRAM]->bind();
        m_programs[MAIN_OBJECT_PROGRAM]->setUniformValue(uniform_v, m_cam);
        m_programs[MAIN_OBJECT_PROGRAM]->setUniformValue(uniform_p, m_proj);
        QMatrix4x4 v_inv = m_cam.inverted();
        m_programs[MAIN_OBJECT_PROGRAM]->setUniformValue(uniform_v_inv, v_inv);
        m_programs[MAIN_OBJECT_PROGRAM]->setUniformValue(uniform_m, Transform*scal*translation);
        QMatrix3x3 m_3x3_inv_transp = Transform.normalMatrix();
        m_programs[MAIN_OBJECT_PROGRAM]->setUniformValue(uniform_m_3x3_inv_transp, m_3x3_inv_transp);

        //Draw the object to analyze
        if(this->selectPlanes || this->isMoverAnillo){
            m_container->drawObjects(m_programs[MAIN_OBJECT_PROGRAM],
                                     indiceCara,
                                     posicionCara,
                                     isMoverCara,
                                     isResizeCara,
                                     m_container->mapcolor_X_Edit,
                                     m_container->mapcolor_Y_Edit,
                                     m_container->mapcolor_Z_Edit);

            //SimpleMesh::Mesh* objeto =  (SimpleMesh::Mesh *)m_container->m_objects.at(0);
        }else{
            m_container->drawObjects(m_programs[MAIN_OBJECT_PROGRAM],
                                     indiceCara,
                                     posicionCara,
                                     isMoverCara,
                                     isResizeCara,
                                     m_container->mapcolor_X,
                                     m_container->mapcolor_Y,
                                     m_container->mapcolor_Z);
        }
    m_programs[MAIN_OBJECT_PROGRAM]->release();


    m_programs[COLORBAR_PROGRAM]->bind();
        QMatrix4x4 un_m = Transform*scal*translation;
        m_programs[COLORBAR_PROGRAM]->setUniformValue(uniform_mvp, m_proj*m_cam*un_m);

        //Draw the color bar object
        if(this->selectPlanes || this->isMoverAnillo){
            m_container->drawColorBar(m_programs[COLORBAR_PROGRAM],
                                      indiceCara,
                                      posicionCara,
                                      isMoverCara,
                                      isResizeCara,
                                      m_container->mapcolor_X_Edit,
                                      m_container->mapcolor_Y_Edit,
                                      m_container->mapcolor_Z_Edit);
        }else{
            m_container->drawColorBar(m_programs[COLORBAR_PROGRAM],
                                      indiceCara,
                                      posicionCara,
                                      isMoverCara,
                                      isResizeCara,
                                      m_container->mapcolor_X,
                                      m_container->mapcolor_Y,
                                      m_container->mapcolor_Z);
        }

    m_programs[COLORBAR_PROGRAM]->release();

    if(rebuild){
        SimpleMesh::Plano3D planoX = m_container->listadoPlanosX[m_container->indiceActualPlanoX];
        SimpleMesh::Plano3D planoY = m_container->listadoPlanosY[m_container->indiceActualPlanoY];
        SimpleMesh::Plano3D planoZ = m_container->listadoPlanosZ[m_container->indiceActualPlanoZ];

        /*cout << "La ecuación del plano X es a = " << planoX.a << " b = " << planoX.b << " c = " << planoX.c << " d = " << planoX.d << endl;
        cout << "La ecuación del plano Y es a = " << planoY.a << " b = " << planoY.b << " c = " << planoY.c << " d = " << planoY.d << endl;
        cout << "La ecuación del plano Z es a = " << planoZ.a << " b = " << planoZ.b << " c = " << planoZ.c << " d = " << planoZ.d << endl;*/
    }

    glFlush();
    cout << "Finish paintGL" << endl;
    //QString valor = MainWindow.getValorTeclado();
    //cout << string(valor) << endl;

}

void GLWidget::resizeGL(int width, int height){
    screen_width = width;
    screen_height = height;
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(width) / height, 0.01f, 100.0f);
    ArcBall.setBounds((GLfloat)width, (GLfloat)height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    isClicked = true;
    LastRot = ThisRot;
    Point2fT auxPoint;
    auxPoint.s.X = (GLfloat)event->pos().x();
    auxPoint.s.Y = (GLfloat)event->pos().y();
    ArcBall.click(&auxPoint);
    setFocus();
    update();
}


void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(isClicked){
        //cout << "Click realized" << endl;
        Quat4fT ThisQuat;
        Point2fT auxPoint;
        auxPoint.s.X = (GLfloat)event->pos().x();
        auxPoint.s.Y = (GLfloat)event->pos().y();
        ArcBall.drag(&auxPoint, &ThisQuat);
        Matrix3fT mThisRot;
        memcpy(mThisRot.M, ThisRot.data(), sizeof(float)*9);

        Matrix3fT mLastRot;
        memcpy(mLastRot.M, LastRot.data(), sizeof(float)*9);
        Matrix4fT mTransform;
        memcpy(mTransform.M, Transform.data(), sizeof(float)*16);

        Matrix3fSetRotationFromQuat4f(&mThisRot, &ThisQuat);
        Matrix3fMulMatrix3f(&mThisRot, &mLastRot);
        Matrix4fSetRotationFromMatrix3f(&mTransform, &mThisRot);

        memcpy(ThisRot.data(), mThisRot.M, sizeof(float)*9);
        memcpy(LastRot.data(), mLastRot.M, sizeof(float)*9);
        memcpy(Transform.data(), mTransform.M, sizeof(float)*16);
        update();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    isClicked = false;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    //float delta = (float)event->delta();
    //delta_scale = delta/12000.0;
    //update();
}
