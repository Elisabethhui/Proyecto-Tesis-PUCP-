! include(../common.pri){
    error("Could not add common config");
}

CONFIG += dll
CONFIG -= app_bundle

#QT       -= core gui

TARGET = HeatDiffusionSymmetry
TEMPLATE = lib

DEFINES += HEATDIFFUSIONSYMMETRY_LIBRARY

SOURCES += src/heatdiffusionsymmetry.cpp

HEADERS += include/HeatDiffusionSymmetry/heatdiffusionsymmetry.h\
        include/HeatDiffusionSymmetry/heatdiffusionsymmetry_global.h

INCLUDEPATH += ./include
INCLUDEPATH += ../Util/include
INCLUDEPATH += ../HeatDiffusionLib/include
INCLUDEPATH += ../SimpleMesh/include
INCLUDEPATH += ../sparseicp/nanoflann-1.1.7/include
INCLUDEPATH += ../Geodesic/include
INCLUDEPATH += ../AdaptiveClustering/include

LIBS += -LC:/Users/Admin/Documents/Sipiran/Implementations/tutorial_opengl/Qt/SymmetryApplication/bin/release -lSimpleMesh -lUtil -lHeatDiffusion -lGeodesic -lAdaptiveClustering
LIBS += -LC:/Users/Admin/Documents/Software/openvdb_3_1_0_library/openvdb/build -lOpenVDB
