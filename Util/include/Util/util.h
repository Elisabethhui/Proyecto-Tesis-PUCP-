#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include "util_global.h"

namespace Util{

#ifdef _cplusplus

extern "C"{
#endif // _cplusplus

    UTIL_API void myPause();
    UTIL_API void myPause(const char* message);
    UTIL_API int sgn(double num);
    UTIL_API void skipline(std::istream &in);
    UTIL_API bool isNumber(std::string str);

    #ifdef _cplusplus
        }
    #endif

template<typename T>
class UTIL_API SortIndex{
public:
    T info;
    int index;

    SortIndex(){info = 0; index = -1;}
    SortIndex(T inf, int ind){info = inf; index = ind;}

    bool operator()(SortIndex a, SortIndex b){return (a.info < b.info);}
};
}
#endif
