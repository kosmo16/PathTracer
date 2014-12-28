#include "BlinnPhongBrdf.h"

#include <cmath>

using namespace std;

const int BlinnPhongBrdf::n;
const float BlinnPhongBrdf::R_d;

BlinnPhongBrdf::BlinnPhongBrdf()
{
}

BlinnPhongBrdf::~BlinnPhongBrdf()
{
}

float BlinnPhongBrdf::computeRatio(
        const Vector3 & incomingDirection,
        const Vector3 & outcomingDirection,
        const Vector3 & normal) const
{
    const Vector3 & N = normal; // normal vector
    const Vector3 & L = incomingDirection; // light vector
    const Vector3 & E = outcomingDirection; // outcoming direction
    const Vector3 H = 0.5f * (L + E); // halfway vector

    float normalizationFactor = 0.0397436 * n + 0.0856832;
    float f0 = fZero(n);

    float rho = R_d * M_1_PI * (1.0f - fDiff(f0, N, L)) + normalizationFactor * fSpec(f0, E, H) * power(N.DotProduct(H), n) / max(N.DotProduct(L), N.DotProduct(E));
    return rho;
}

float BlinnPhongBrdf::fDiff(float f0, const Vector3 & N, const Vector3 & L)
{
    return FresnelFunction(f0, N, L);
}

float BlinnPhongBrdf::fSpec(float f0, const Vector3 & E, const Vector3 & H)
{
    return FresnelFunction(f0, E, H);
}
