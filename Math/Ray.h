#ifndef RAY_H
#define RAY_H

#include "Vector4.h"

#include <QDebug>

class Geometry;

class Ray
{
public:
    Vector3 origin;
    Vector3 direction;

    Ray();
    Ray(const Vector3 &orig, const Vector3 &dir);
    Ray(const Vector4 &orig, const Vector4 &dir);
};

enum IntersectionType {
    MISS = 0,
    HIT,
    INSIDE_PRIMITIVE
};

struct IntersectionResult {
    IntersectionResult() {
        type=MISS;
        object=0;
    }

    IntersectionResult(IntersectionType t,
                       Vector3 p,
                       Vector3 n,
                       float dist) : type(t), LPOINT(p),
                                     intersectionLPOINTNormal(n), distance(dist) {}
    IntersectionType type;
    Vector3 LPOINT;
    Vector3 intersectionLPOINTNormal;
    float distance;
    const Geometry* object;
};


QDebug operator<<(QDebug dbg, const Ray &r);
QDebug operator<<(QDebug dbg, const IntersectionResult &ir);

#endif // RAY_H
