#ifndef _VECTOR3_H
#define _VECTOR3_H
//------------------------------------------------------------------------------
/**
    @class _vector3
    @ingroup Math

    Generic vector3 class. Uses 16 Byte of mem instead of 12 (!)

    (C) 2002 RadonLabs GmbH
*/
#include "Util/util_global.h"
#include "nmath.h"
#include <float.h>

namespace Util{


class UTIL_API _vector3
{
public:
    /// constructor 1
    _vector3();
    /// constructor 2
    _vector3(const float _x, const float _y, const float _z);
    /// constructor 3
    _vector3(const _vector3& vec);
    /// set elements 1
    void set(const float _x, const float _y, const float _z);
    /// set elements 2
    void set(const _vector3& vec);
    /// return length
    float len() const;
    /// return length squared
    float lensquared() const;
    /// normalize
    void norm();
    /// inplace add
    void operator +=(const _vector3& v0);
    /// inplace sub
    void operator -=(const _vector3& v0);
    /// inplace scalar multiplication
    void operator *=(float s);
    /// true if any of the elements are greater
    bool operator >(const _vector3& rhs);
    /// true if any of the elements are smaller
    bool operator <(const _vector3& rhs);
    /// fuzzy compare, return true/false
    bool isequal(const _vector3& v, float tol) const;
    /// fuzzy compare, returns -1, 0, +1
    int compare(const _vector3& v, float tol) const;
    /// rotate around axis
    void rotate(const _vector3& axis, float angle);
    /// inplace linear interpolation
    void lerp(const _vector3& v0, float lerpVal);
    /// returns a vector orthogonal to self, not normalized
    _vector3 findortho() const;
    /// saturate components between 0 and 1
    void saturate();
    /// dot product
    float dot(const _vector3& v0) const;
    /// distance between 2 vector3's
    static float distance(const _vector3& v0, const _vector3& v1);

    float x, y, z;
};

}


#endif
