#ifndef GEODESIC_GLOBAL_H
#define GEODESIC_GLOBAL_H

//Helper macros to define library symbol visibility
#ifdef GEODESIC_EXPORT
#undef GEODESIC_EXPORT
#endif

#ifdef GEODESIC_IMPORT
#undef GEODESIC_IMPORT
#endif

#ifdef _MSC_VER
    #if defined(_DLL) && !defined(GEODESIC_STATICLIB) && !defined(GEODESIC_DLL)
        #define GEODESIC_DLL
    #endif
#endif

#ifdef __GNUC__
    #define GEODESIC_EXPORT __attribute__((visibility("default")))
    #define GEODESIC_IMPORT __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
    #ifdef GEODESIC_DLL
        #define GEODESIC_EXPORT __declspec(dllexport)
        #define GEODESIC_IMPORT __declspec(dllimport)
    #else
        #define GEODESIC_EXPORT
        #define GEODESIC_IMPORT
    #endif
#endif

#ifdef GEODESIC_API
#undef GEODESIC_API
#endif

#ifdef GEODESIC_LIBRARY
    #define GEODESIC_API GEODESIC_EXPORT
#else
    #define GEODESIC_API GEODESIC_IMPORT
#endif

#endif // GEODESIC_GLOBAL_H
