#include "normalPdf.h"

NormalPdf::NormalPdf()
{
}

NormalPdf::~NormalPdf()
{
}

// -PI_2 <= f <= PI_2
float randomFloat()
{
    static const float F_RAND_MAX = float(RAND_MAX);
    static const float F_RAND_MAX_2 = 0.5f * F_RAND_MAX;
    static const float FACTOR = M_PI_2 / F_RAND_MAX;

    // f = (rand() - 0.5f * RAND_MAX) * (M_PI_2 / RAND_MAX)
    float f = rand();
    f -= F_RAND_MAX_2;
    f *= FACTOR;
    return f;
}

Vector3 rotate(float angle, Vector3 axis, const Vector3 & v)
{
    // https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml

    axis.Normalize();

    float s = sin(angle);
    float c = cos(angle);

    float one_c = 1.0f - c;

    float xx = axis.x * axis.x;
    float xy = axis.x * axis.y;
    float xz = axis.x * axis.z;
    float yy = axis.y * axis.y;
    float yz = axis.y * axis.z;
    float zz = axis.z * axis.z;

    float sx = axis.x * s;
    float sy = axis.y * s;
    float sz = axis.z * s;

    Matrix4x4 m(xx * one_c + c, xy * one_c - sz, xz * one_c + sy, 0.0f,
               xy * one_c + sz, yy * one_c + c, yz * one_c - sx, 0.0f,
               xz * one_c - sy, yz * one_c + sx, zz * one_c + c, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f);
    return m * v;
}

Vector3 NormalPdf::computeDirection(const Vector3 & incomingDirection, const Vector3 & normal) const
{
    const Vector3 & V = incomingDirection;
    const Vector3 & N = normal;

    // wektor normalny plaszczyzny odbicia
    const Vector3 A = V.CrossProduct(N);

    // parametr ~ kat wzgledem normalnej
    // -PI_2 <= t <= PI_2
    float t = randomFloat();

    const Vector3 & outcomingDirection = rotate(t, A, N);
    return outcomingDirection;
}
