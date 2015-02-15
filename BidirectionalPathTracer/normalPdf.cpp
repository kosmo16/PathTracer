#include "normalPdf.h"

#include "Math/randomUtils.h"

NormalPdf::NormalPdf()
{
}

NormalPdf::~NormalPdf()
{
}

Vector3 NormalPdf::computeDirection(const Vector3 & incomingDirection, const Vector3 & normal) const
{
    const Vector3 & V = incomingDirection;
    const Vector3 & N = normal;

    // wektor normalny plaszczyzny odbicia
    const Vector3 A = V.CrossProduct(N);

    // parametr ~ kat wzgledem normalnej
    // -PI_2 <= t <= PI_2
    float t = randomSignedFloat(M_PI_2);
    // float t = randomFloat();

    const Vector3 & outcomingDirection = rotate(t, A, N);
    return outcomingDirection;
}
