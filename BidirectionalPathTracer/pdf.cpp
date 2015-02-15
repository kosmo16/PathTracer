#include "pdf.h"

Pdf::Pdf()
{
}

Pdf::~Pdf()
{
}

Vector3 Pdf::rotate(float angle, Vector3 axis, const Vector3 & v)
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
