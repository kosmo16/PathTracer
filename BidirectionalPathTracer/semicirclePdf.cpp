#include "semicirclePdf.h"

#include "Math/randomUtils.h"

SemicirclePdf::SemicirclePdf()
{
}

SemicirclePdf::~SemicirclePdf()
{
}

Vector3 SemicirclePdf::computeDirection(const Vector3 &incomingDirection, const Vector3 &normal) const
{
    const Vector3 &outcomingDirection = randomRotate(normal, incomingDirection, normal);
    return outcomingDirection;
}
