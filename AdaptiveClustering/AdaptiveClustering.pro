! include(../common.pri){
    error("Could not add common config");
}

CONFIG += dll
CONFIG -= app_bundle
CONFIG -= qt

TARGET = AdaptiveClustering
TEMPLATE = lib

INCLUDEPATH += include

HEADERS +=  include/AdaptiveClustering/adaptive_clustering_global.h \
            include/AdaptiveClustering/adaptiveClustering.h \
            include/AdaptiveClustering/cluster.h \
            include/AdaptiveClustering/Point.h \
            include/AdaptiveClustering/PointND.h

SOURCES += src/adaptiveClustering.cpp \
            src/cluster.cpp

DEFINES += ADAPTIVE_LIBRARY


