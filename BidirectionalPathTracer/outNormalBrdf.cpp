#include "outNormalBrdf.h"

OutNormalBrdf::OutNormalBrdf()
{
}

OutNormalBrdf::~OutNormalBrdf()
{
}

float OutNormalBrdf::computeRatio(const Vector3 &incomingDirection, const Vector3 &outcomingDirection, const Vector3 &normal) const
{
    float ratio = 2.0f * outcomingDirection.DotProduct(normal);
    return ratio;
}
