
TARGET = trimeshinfo
QMAKE_LIBDIR += 
DEPENDPATH += . 
INCLUDEPATH += . ../..
CONFIG += console stl
TEMPLATE = app
HEADERS += 
SOURCES += trimeshinfo.cpp ../../wrap/ply/plylib.cpp
