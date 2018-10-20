#ifndef HEATDIFFUSION_GLOBAL_H
#define HEATDIFFUSION_GLOBAL_H

//Helper macros to define library symbol visibility
#ifdef HEATDIFFUSION_EXPORT
#undef HEATDIFFUSION_EXPORT
#endif

#ifdef HEATDIFFUSION_IMPORT
#undef HEATDIFFUSION_IMPORT
#endif

#ifdef _MSC_VER
    #if defined(_DLL) && !defined(HEATDIFFUSION_STATICLIB) && !defined(HEATDIFFUSION_DLL)
        #define HEATDIFFUSION_DLL
    #endif
#endif

#ifdef __GNUC__
    #define HEATDIFFUSION_EXPORT __attribute__((visibility("default")))
    #define HEATDIFFUSION_IMPORT __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
    #ifdef HEATDIFFUSION_DLL
        #define HEATDIFFUSION_EXPORT __declspec(dllexport)
        #define HEATDIFFUSION_IMPORT __declspec(dllimport)
    #else
        #define HEATDIFFUSION_EXPORT
        #define HEATDIFFUSION_IMPORT
    #endif
#endif

#ifdef HEATDIFFUSION_API
#undef HEATDIFFUSION_API
#endif

#ifdef HEATDIFFUSION_LIBRARY
    #define HEATDIFFUSION_API HEATDIFFUSION_EXPORT
#else
    #define HEATDIFFUSION_API HEATDIFFUSION_IMPORT
#endif

#endif // HEATDIFFUSION_GLOBAL_H
