#ifndef ADAPTIVE_CLUSTERING_GLOBAL_H_INCLUDED
#define ADAPTIVE_CLUSTERING_GLOBAL_H_INCLUDED

//Helper macros to define library symbol visibility
#ifdef ADAPTIVE_EXPORT
#undef ADAPTIVE_EXPORT
#endif

#ifdef ADAPTIVE_IMPORT
#undef ADAPTIVE_IMPORT
#endif

#ifdef _MSC_VER
#if defined(_DLL) && !defined(ADAPTIVE_STATICLIB) && !defined(ADAPTIVE_DLL)
#define ADAPTIVE_DLL
#endif
#endif

#ifdef __GNUC__
#define ADAPTIVE_EXPORT __attribute__((visibility("default")))
#define ADAPTIVE_IMPORT __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
#ifdef ADAPTIVE_DLL
#define ADAPTIVE_EXPORT __declspec(dllexport)
#define ADAPTIVE_IMPORT __declspec(dllimport)
#else
#define ADAPTIVE_EXPORT
#define ADAPTIVE_IMPORT
#endif
#endif

#ifdef ADAPTIVE_API
#undef ADAPTIVE_API
#endif

#ifdef ADAPTIVE_LIBRARY
#define ADAPTIVE_API ADAPTIVE_EXPORT
#else
#define ADAPTIVE_API ADAPTIVE_IMPORT
#endif

#endif // ADAPTIVE_CLUSTERING_GLOBAL_H_INCLUDED
