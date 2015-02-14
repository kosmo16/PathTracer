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

    float fDiff_f0_N_L = fDiff(f0, N, L);
    float left = R_d * M_1_PI * (1.0f - fDiff_f0_N_L);

    float fSpec_f0_E_H = fSpec(f0, E, H);
    float NdotH = N.DotProduct(H);
    float NdotL = N.DotProduct(L);
    float NdotE = N.DotProduct(E);
    float right = normalizationFactor * fSpec_f0_E_H * power(NdotH, n) / max(NdotL, NdotE);

    float rho = left + right;
    if(rho < 0.0f)
    {
        qDebug().nospace() << __LINE__ << ". R_d: " << R_d;
        qDebug().nospace() << __LINE__ << ". f0: " << f0;

        qDebug().nospace() << __LINE__ << ". fDiff_f0_N_L: " << fDiff_f0_N_L;
        qDebug().nospace() << __LINE__ << ". left: " << left;

        qDebug().nospace() << __LINE__ << ". fSpec_f0_E_H: " << fSpec_f0_E_H;
        qDebug().nospace() << __LINE__ << ". NdotH: " << NdotH;
        qDebug().nospace() << __LINE__ << ". NdotL: " << NdotL;
        qDebug().nospace() << __LINE__ << ". NdotE: " << NdotE;
        qDebug().nospace() << __LINE__ << ". right: " << right;

        qDebug().nospace() << __LINE__ << ". rho: " << rho;
        throw 1u;
    }
    if(rho > 1.0f)
    {
        throw 1l;
    }
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
