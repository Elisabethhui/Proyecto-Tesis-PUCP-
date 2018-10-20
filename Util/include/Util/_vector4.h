#ifndef _VECTOR4_H
#define _VECTOR4_H
//------------------------------------------------------------------------------
/**
    @class _vector4
    @ingroup Math

    A generic vector4 class.

    (C) 2002 RadonLabs GmbH
*/
#include "Util/util_global.h"
#include "nmath.h"
#include "_vector3.h"
#include <float.h>

namespace Util{


class UTIL_API _vector4
{
public:
    enum component
    {
        X = (1<<0),
        Y = (1<<1),
        Z = (1<<2),
        W = (1<<3),
    };

    /// constructor 1
    _vector4();
    /// constructor 2
    _vector4(const float _x, const float _y, const float _z, const float _w);
    /// constructor 3
    _vector4(const _vector4& vec);
    /// constructor from vector3 (w will be set to 1.0)
    _vector4(const _vector3& vec3);
    /// set elements 1
    void set(const float _x, const float _y, const float _z, const float _w);
    /// set elements 2
    void set(const _vector4& v);
    /// set to vector3 (w will be set to 1.0)
    void set(const _vector3& v);
    /// return length
    float len() const;
    /// normalize
    void norm();
    /// inplace add
    void operator +=(const _vector4& v);
    /// inplace sub
    void operator -=(const _vector4& v);
    /// inplace scalar mul
    void operator *=(const float s);
    /// vector3 assignment operator (w set to 1.0f)
    _vector4& operator=(const _vector3& v);
    /// fuzzy compare
    bool isequal(const _vector4& v, float tol) const;
    /// fuzzy compare, return -1, 0, +1
    int compare(const _vector4& v, float tol) const;
    /// set own components to minimum
    void minimum(const _vector4& v);
    /// set own components to maximum
    void maximum(const _vector4& v);
    /// set component float value by mask
    void setcomp(float val, int mask);
    /// get component float value by mask
    float getcomp(int mask);
    /// get write mask for smallest component
    int mincompmask() const;
    /// inplace linear interpolation
    void lerp(const _vector4& v0, float lerpVal);
    /// saturate components between 0 and 1
    void saturate();
    /// dot product
    float dot(const _vector4& v0) const;

    float x, y, z, w;
};

}


#endif

