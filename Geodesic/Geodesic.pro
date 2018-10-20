#-------------------------------------------------
#
# Project created by QtCreator 2017-02-14T11:12:17
#
#-------------------------------------------------

! include(../common.pri){
    error("Could not add common config");
}

CONFIG += dll
QT       -= gui

TARGET = Geodesic
TEMPLATE = lib

DEFINES += GEODESIC_LIBRARY

SOURCES += src/geodesic.cpp \
    src/gw_core/GW_Config.cpp \
    src/gw_core/GW_Face.cpp \
    src/gw_core/GW_FaceIterator.cpp \
    src/gw_core/GW_Mesh.cpp \
    src/gw_core/GW_SmartCounter.cpp \
    src/gw_core/GW_Vertex.cpp \
    src/gw_core/GW_VertexIterator.cpp \
    src/gw_geodesic/GW_GeodesicFace.cpp \
    src/gw_geodesic/GW_GeodesicMesh.cpp \
    src/gw_geodesic/GW_GeodesicPath.cpp \
    src/gw_geodesic/GW_GeodesicPoint.cpp \
    src/gw_geodesic/GW_GeodesicVertex.cpp \
    src/gw_geodesic/GW_TriangularInterpolation_Cubic.cpp \
    src/gw_geodesic/GW_TriangularInterpolation_Linear.cpp \
    src/gw_geodesic/GW_TriangularInterpolation_Quadratic.cpp \
    src/gw_geodesic/GW_VoronoiVertex.cpp

HEADERS += include/Geodesic/geodesic_global.h \
    include/Geodesic/gw_core/GW_Config.h \
    include/Geodesic/gw_core/GW_Face.h \
    include/Geodesic/gw_core/GW_FaceIterator.h \
    include/Geodesic/gw_core/GW_MathsWrapper.h \
    include/Geodesic/gw_core/GW_Mesh.h \
    include/Geodesic/gw_core/GW_PolygonIntersector.h \
    include/Geodesic/gw_core/GW_ProgressBar.h \
    include/Geodesic/gw_core/GW_Serializable.h \
    include/Geodesic/gw_core/GW_SmartCounter.h \
    include/Geodesic/gw_core/GW_Vertex.h \
    include/Geodesic/gw_core/GW_VertexIterator.h \
    include/Geodesic/gw_geodesic/GW_GeodesicFace.h \
    include/Geodesic/gw_geodesic/GW_GeodesicMesh.h \
    include/Geodesic/gw_geodesic/GW_GeodesicPath.h \
    include/Geodesic/gw_geodesic/GW_GeodesicPoint.h \
    include/Geodesic/gw_geodesic/GW_GeodesicVertex.h \
    include/Geodesic/gw_geodesic/GW_GeometryAtlas.h \
    include/Geodesic/gw_geodesic/GW_GeometryCell.h \
    include/Geodesic/gw_geodesic/GW_Parameterization.h \
    include/Geodesic/gw_geodesic/GW_TriangularInterpolation.h \
    include/Geodesic/gw_geodesic/GW_TriangularInterpolation_ABC.h \
    include/Geodesic/gw_geodesic/GW_TriangularInterpolation_Cubic.h \
    include/Geodesic/gw_geodesic/GW_TriangularInterpolation_Linear.h \
    include/Geodesic/gw_geodesic/GW_TriangularInterpolation_Quadratic.h \
    include/Geodesic/gw_geodesic/GW_VoronoiMesh.h \
    include/Geodesic/gw_geodesic/GW_VoronoiVertex.h \
    include/Geodesic/gw_maths/gw_complex.h \
    include/Geodesic/gw_maths/GW_Maths.h \
    include/Geodesic/gw_maths/GW_MathsConfig.h \
    include/Geodesic/gw_maths/GW_Matrix2x2.h \
    include/Geodesic/gw_maths/GW_Matrix3x3.h \
    include/Geodesic/gw_maths/GW_Matrix4x4.h \
    include/Geodesic/gw_maths/GW_MatrixNxP.h \
    include/Geodesic/gw_maths/GW_MatrixStatic.h \
    include/Geodesic/gw_maths/GW_Quaternion.h \
    include/Geodesic/gw_maths/GW_SparseMatrix.h \
    include/Geodesic/gw_maths/GW_Vector2D.h \
    include/Geodesic/gw_maths/GW_Vector3D.h \
    include/Geodesic/gw_maths/GW_Vector4D.h \
    include/Geodesic/gw_maths/GW_VectorND.h \
    include/Geodesic/gw_maths/GW_VectorStatic.h \
    include/Geodesic/geodesic.h

INCLUDEPATH += C:\Users\Admin\Documents\Sipiran\Implementations\tutorial_opengl\Qt\SymmetryApplication\Geodesic\include
INCLUDEPATH += ../SimpleMesh/include
