#include "pdf.h"

#include "Math/randomUtils.h"

Pdf::Pdf()
{
}

Pdf::~Pdf()
{
}

Vector3 Pdf::rotate(const Vector3 &v, const Vector3 &axis, float angle)
{
    float axisSquaredLength = axis.GetSquaredLength();
    static const float Epsilon = 0.001f;
    static const float mini = 1.0f - Epsilon;
    static const float maxi = 1.0f + Epsilon;
    if(axisSquaredLength < mini|| axisSquaredLength > maxi)
    {
        throw string("os musi byc znormalizowana");
    }

    // https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml

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

const Vector3& Pdf::randomRotate(const Vector3 &vectorToRotate, const Vector3 &axis)
{
    // kat obrotu
    // -PI_2 <= t <= PI_2
    float t = randomSignedFloat(M_PI_2);

    const Vector3 &rotatedVector = rotate(vectorToRotate, axis, t);
    return rotatedVector;
}

const Vector3& Pdf::randomRotate(const Vector3 &vectorToRotate, const Vector3 &planeVector1, const Vector3 &planeVector2)
{
    // wektor normalny plaszczyzny
    const Vector3 A = planeVector1.CrossProduct(planeVector2);

    const Vector3 &rotatedVector = randomRotate(vectorToRotate, A);
    return rotatedVector;
}
