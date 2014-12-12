#ifndef __BRDF_H__
#define __BRDF_H__

#include "Math/Vector3.h"

template<typename TX, typename TP> TX power(TX x, TP p, const TX & zero = TX(0), const TX & one = TX(1))
{
    const TP zeroP = TP(0);
    const TP oneP = TP(1);

    if(x == zero && p == zeroP)
        throw false;

    if(p == zeroP || x == one)
        return one;
    if(x == zero)
        return zero;

    TX w = p & oneP ? x : one;
    for(p >>= oneP; p > zeroP; p >>= oneP)
    {
        x *= x;
        if(p & oneP)
            w *= x;
    }
    return w;
}

class Brdf
{
protected:
    static float to5power(float x);
    static float FresnelFunction(float f0, const Vector3 & A, const Vector3 & B);

    static float fZero(int n) const;

public:
    Brdf();

    virtual float computeRatio(const Vector3 & incomingDirection, const Vector3 & outcomingDirection) const = 0;
};

#endif
