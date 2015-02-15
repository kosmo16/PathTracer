#ifndef __PDF_H__
#define __PDF_H__

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

class Pdf
{
protected:
    static Vector3 rotate(float angle, Vector3 axis, const Vector3 & v);

public:
    Pdf();
    virtual ~Pdf();

    virtual Vector3 computeDirection(const Vector3 & incomingDirection, const Vector3 & normal) const = 0;
};

#endif
