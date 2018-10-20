! include(../common.pri){
    error("Could not add common config");
}

CONFIG += dll
CONFIG -= app_bundle
CONFIG -= qt

TARGET = HeatDiffusion
TEMPLATE = lib

INCLUDEPATH += include/
INCLUDEPATH += ../SimpleMesh/include

HEADERS += include/HeatDiffusion/HeatDiffusion_global.h \
           include/HeatDiffusion/Laplacian.h \
           include/HeatDiffusion/Descriptor/HeatKernelSignature.h \
           include/HeatDiffusion/Descriptor/WaveKernelSignature.h \
           include/HeatDiffusion/Distance/BiharmonicDistance.h \
           include/HeatDiffusion/Distance/ConmuteTimeDistance.h \
           include/HeatDiffusion/Distance/DiffusionDistance.h

SOURCES +=  src/Laplacian.cpp \
            src/HeatKernelSignature.cpp \
            src/WaveKernelSignature.cpp \
            src/BiharmonicDistance.cpp \
            src/ConmuteTimeDistance.cpp \
            src/DiffusionDistance.cpp

DEFINES += HEATDIFFUSION_LIBRARY


#NANOFLANN
INCLUDEPATH += ./Third-party/nanoflann-1.1.7/include
INCLUDEPATH += C:\Users\Admin\Documents\Sipiran\PRESIOUS\Implementations\Completion-Console\Completion-console\Third-party\arpack++\include

LIBS += -L$${DESTDIR} -lSimpleMesh
LIBS += -LC:\Users\Admin\Documents\Software\ARPACK -larpack_win64
LIBS += "C:\Users\Admin\Documents\Visual Studio 2013\Projects\SuperLU43\x64\Release\SuperLU43.lib"
LIBS += -L"C:\Users\Admin\Documents\Software\OpenBLAS 0.2.18 version\xianyi-OpenBLAS-3f6398a\exports" -llibopenblas
