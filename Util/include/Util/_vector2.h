#ifndef _VECTOR2_H
#define _VECTOR2_H
//------------------------------------------------------------------------------
/**
    @class _vector2
    @ingroup Math

    Generic vector2 class.

    (C) 2002 RadonLabs GmbH
*/

#include "Util/util_global.h"
#include "nmath.h"
#include <float.h>

namespace Util {

class UTIL_API _vector2 {
public:
    /// constructor 1
    _vector2();
    /// constructor 2
    _vector2(const float _x, const float _y);
    /// constructor 3
    _vector2(const _vector2& vec);
    /// constructor 4
    _vector2(const float* p);
    /// set elements 1
    void set(const float _x, const float _y);
    /// set elements 2
    void set(const _vector2& vec);
    /// set elements 3
    void set(const float* p);
    /// return length
    float len() const;
    /// normalize
    void norm();
    /// in place add
    void operator+=(const _vector2& v0);
    /// in place sub
    void operator-=(const _vector2& v0);
    /// in place scalar mul
    void operator*=(const float s);
    /// in place scalar div
    void operator/=(const float s);
    /// fuzzy compare operator
    bool isequal(const _vector2& v, const float tol) const;
    /// fuzzy compare, returns -1, 0, +1
    int compare(const _vector2& v, float tol) const;
    /// rotate around P(0,0)
    void rotate(float angle);

    float x, y;
};

}
#endif

