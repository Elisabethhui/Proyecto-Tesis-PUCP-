! include(../common.pri){
    error("Could not add common config");
}

CONFIG += dll
CONFIG -= app_bundle
CONFIG -= qt

TARGET = Util
TEMPLATE = lib

INCLUDEPATH += include

HEADERS +=  include/Util/util_global.h \
            include/Util/PropertySet.h \
            include/Util/rotation.h \
            include/Util/core.h \
            include/Util/simplify.h \
            include/Util/vecmath.h \
            include/Util/_vector2.h \
            include/Util/_vector3.h \
            include/Util/_vector4.h \
            include/Util/nmath.h \
            include/Util/ntypes.h \
            include/Util/vector.h \
           include/Util/util.h

SOURCES += src/PropertySet.cpp \
           src/rotation.cpp \
            src/core.cpp \
            src/vecmath.cpp \
           src/util.cpp \
    src/_vector2.cpp \
    src/_vector3.cpp \
    src/_vector4.cpp \
    src/nmath.cpp \
    src/simplify.cpp

DEFINES += UTIL_LIBRARY


