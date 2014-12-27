#ifndef __PDF_H__
#define __PDF_H__

#include "Math/Vector3.h"

class Pdf
{
public:
    Pdf();

    virtual Vector3 computeDirection(const Vector3 & incomingDirection, const Vector3 & normal) const = 0;
};

#endif
