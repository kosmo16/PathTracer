#include "hemispherePdf.h"

HemispherePdf::HemispherePdf()
{
}

HemispherePdf::~HemispherePdf()
{
}

Vector3 HemispherePdf::computeDirection(const Vector3 &incomingDirection, const Vector3 &normal) const
{
    Vector3 outcomingDirection = normal;

    const Vector3 &A = normal.CrossProduct(incomingDirection);

    outcomingDirection = randomRotate(outcomingDirection, A);
    outcomingDirection = randomRotate(outcomingDirection, A, normal);

    return outcomingDirection;
}
