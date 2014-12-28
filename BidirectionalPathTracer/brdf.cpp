#include "brdf.h"

Brdf::Brdf()
{
}

Brdf::~Brdf()
{
}

float Brdf::fZero(int n)
{
    float f0 = float(1 - n) / float(1 + n);
    f0 *= f0;
    return f0;
}

float Brdf::to5power(float x)
{
    float power = x * x;
    power *= power;
    power *= x;
    return power;
}

float Brdf::FresnelFunction(float f0, const Vector3 & A, const Vector3 & B)
{
    return f0 + (1.0f - f0) * to5power(1.0f - A.DotProduct(B));
}
