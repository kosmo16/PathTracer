#include "randomUtils.h"

#include <QtGlobal>

#include <cmath>
#include <cstdlib>

static const float F_RAND_MAX = float(RAND_MAX);
static const float F_RAND_MAX_2 = 0.5f * F_RAND_MAX;

// -PI_2 <= f <= PI_2
float randomFloatAngle()
{
    static const float FACTOR = M_PI / F_RAND_MAX;

    // f = (qrand() - 0.5f * RAND_MAX) * (M_PI / RAND_MAX)
    float f = qrand();
    f -= F_RAND_MAX_2;
    f *= FACTOR;
    return f;
}

// -0.5 <= f <= 0.5
float randomSignedFloatHalf()
{
    static const float FACTOR = 1.0f / F_RAND_MAX;

    // f = (qrand() - 0.5f * RAND_MAX) / RAND_MAX
    float f = qrand();
    f -= F_RAND_MAX_2;
    f *= FACTOR;
    return f;
}

// -1.0 <= f <= 1.0
float randomSignedFloat()
{
    static const float FACTOR = 2.0f / F_RAND_MAX;

    // f = (qrand() - 0.5f * RAND_MAX) * 2.0f / RAND_MAX
    float f = qrand();
    f -= F_RAND_MAX_2;
    f *= FACTOR;
    return f;
}

// 0.0 <= f <= 1.0
float randomUnsignedFloat()
{
    static const float FACTOR = 1.0f / F_RAND_MAX;

    // f = qrand() / RAND_MAX
    float f = qrand();
    f *= FACTOR;
    return f;
}
