#include "dotBrdf.h"

DotBrdf::DotBrdf()
{
}

DotBrdf::~DotBrdf()
{
}

float DotBrdf::computeRatio(const Vector3 &incomingDirection, const Vector3 &outcomingDirection, const Vector3 &normal) const
{
    float dot = incomingDirection.DotProduct(outcomingDirection);
    float ratio = 1.0f - (dot >= 0.0f ? dot : -dot);
    return ratio;
}
