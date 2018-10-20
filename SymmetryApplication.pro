TEMPLATE = subdirs

VAR_INTERFACES = on

CONFIG += \
	warn on

SUBDIRS += Util

!defined(VAR_INTERFACES, var){
    SUBDIRS += AdaptiveClustering
    SUBDIRS += Harris3D
    SUBDIRS += HeatDiffusionLib
    SUBDIRS += RotationalSymmetry
    SUBDIRS += HeatDiffusionSymmetry
    SUBDIRS += Geodesic
}

SUBDIRS += SimpleMesh
SUBDIRS += SymArch

!defined(VAR_INTERFACES, var){
    Harris3D.depends            += Util SimpleMesh
    HeatDiffusionLib.depends    += SimpleMesh
    RotationalSymmetry          += Util SimpleMesh Harris3D
    HeatDiffusionSymmetry       += Util SimpleMesh HeatDiffusionLib Geodesic AdaptiveClustering
    SymArch                     += RotationalSymmetry HeatDiffusionSymmetry
}

defined(VAR_INTERFACES, var){
    SymArch                     += Util SimpleMesh
}
