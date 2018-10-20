! include(../common.pri){
    error("Could not add common config");
}

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SymArch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    bufferedobject3d.cpp \
    objectcontainer.cpp \
    ArcBall.cpp \
    analysisbar.cpp \
    rotationaldialog.cpp \
    layerdock.cpp \
    progressbarmessage.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    bufferedobject3d.h \
    objectcontainer.h \
    ArcBall.h \
    analysisbar.h \
    rotationaldialog.h \
    layerdock.h \
    progressbarmessage.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\Users\Admin\Documents\Software\glm-0.9.7.3\glm
INCLUDEPATH += D:\mathlab\extern\include

#--------- SimpleMesh---------------------
INCLUDEPATH += ../SimpleMesh/include
INCLUDEPATH += ../Util/include

LIBS += D:/mathlab/extern/lib/win64/microsoft/mclmcrrt.lib
LIBS += D:/mathlab/bin/generatedCode/first/faceDemoMissingData.lib


!defined(VAR_INTERFACES, var){
    INCLUDEPATH += ../Harris3D/include
    INCLUDEPATH += ../RotationalSymmetry/include
    INCLUDEPATH += ../HeatDiffusionSymmetry/include
    INCLUDEPATH += ../sparseicp
    INCLUDEPATH += ../sparseicp/nanoflann-1.1.7/include
    INCLUDEPATH += C:/Users/Admin/Documents/Software/CGAL-release/auxiliary/gmp/include

    LIBS += -LC:/Users/Admin/Documents/Sipiran/Implementations/tutorial_opengl/Qt/SymmetryApplication/bin/release -lSimpleMesh -lUtil -lHarris3D -lRotationalSymmetry -lHeatDiffusion -lHeatDiffusionSymmetry
    LIBS += -LC:/Users/Admin/Documents/Software/openvdb_3_1_0_library/openvdb/build -lOpenVDB
    LIBS += -LC:/Users/Admin/Documents/Software/CGAL-release/auxiliary/gmp/lib -llibgmp
}

defined(VAR_INTERFACES, var){
    LIBS += -LC:/Users/Admin/Documents/Sipiran/Implementations/tutorial_opengl/Qt/SymmetryApplication/bin/release -lSimpleMesh -lUtil
}

DISTFILES += \
    shaders/phong-shading.f.glsl \
    shaders/phong-shading.v.glsl \
    shaders/bounding-shading.v.glsl \
    shaders/bounding-shading.f.glsl \
    shaders/per-vertex-shading.v.glsl \
    shaders/per-vertex-shading.f.glsl

CONFIG += no_keywords
DEFINES += ONLY_INTERFACES
