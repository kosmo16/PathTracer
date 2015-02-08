#include "randomUtils.h"

#include <QtGlobal>

// mini <= f <= maxi
float randomFloat(float mini, float maxi)
{
    static const float F_RAND_MAX = float(RAND_MAX);
    static const float F_1_RAND_MAX = 1.0f / F_RAND_MAX;

    return qrand() * F_1_RAND_MAX * (maxi - mini) + mini;
}

// -maxi <= f <= maxi
float randomSignedFloat(float maxi)
{
    static const float F_RAND_MAX = float(RAND_MAX);
    static const float F_2_RAND_MAX = 2.0f / F_RAND_MAX;

    return qrand() * F_2_RAND_MAX * maxi - maxi;
}

// 0 <= f <= maxi
float randomUnsignedFloat(float maxi)
{
    static const float F_RAND_MAX = float(RAND_MAX);
    static const float F_1_RAND_MAX = 1.0f / F_RAND_MAX;

    return qrand() * F_1_RAND_MAX * maxi;
}
