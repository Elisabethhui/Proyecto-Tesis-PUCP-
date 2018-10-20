#ifndef N_MATH_H
#define N_MATH_H
//------------------------------------------------------------------------------
/**
    General math functions and macros.

    (C) 2003 RadonLabs GmbH
*/
#include "Util/util_global.h"
#include <math.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define isnan _isnan
#define isinf _isinf
#endif

#ifndef PI
#define PI (3.1415926535897932384626433832795028841971693993751f)
#endif

#define N_PI PI

#ifndef TINY
#define TINY (0.0000001f)
#endif

#define n_max(a,b)      (((a) > (b)) ? (a) : (b))
#define n_min(a,b)      (((a) < (b)) ? (a) : (b))
#define n_abs(a)        (((a)<0.0f) ? (-(a)) : (a))
#define n_sgn(a)        (((a)<0.0f) ? (-1) : (1))
#define n_deg2rad(d)    (((d)*PI)/180.0f)
#define n_rad2deg(r)    (((r)*180.0f)/PI)
#define n_sin(x) (float(sin(x)))
#define n_cos(x) (float(cos(x)))
#define n_tan(x) (float(tan(x)))
#define n_atan(x) (float(atan(x)))

namespace Util{

#ifdef _cplusplus

extern "C"{
#endif // _cplusplus

   UTIL_API float n_log2(float);
   UTIL_API int n_iclamp(int, int, int);
   UTIL_API float n_acos(float);
   UTIL_API float n_asin(float);
   UTIL_API float n_sqrt(float);
   UTIL_API bool n_fequal(float, float, float);
   UTIL_API bool n_fless(float, float, float);
   UTIL_API bool n_fgreater(float, float, float);
   UTIL_API long n_ftol(float);
   UTIL_API float n_smooth(float, float, float);
   UTIL_API float n_clamp(float, float, float);
   UTIL_API float n_saturate(float);
   UTIL_API float n_rand();
   UTIL_API int n_fchop(float);
   UTIL_API int n_frnd(float);
   UTIL_API float n_lerp(float, float, float);

    #ifdef _cplusplus
        }
    #endif
}


#endif

