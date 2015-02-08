#include "Sphere.h"

IntersectionResult Sphere::Intersects(const Ray &ray, float range) const {
    IntersectionResult result;
    result.type = MISS;

    Vector3 v = ray.origin - position;
    float b = -v.DotProduct(ray.direction);
    float det = b*b - v.DotProduct(v) + radius*radius;
    if(det > 0) {
        det = sqrtf(det);
        float hitDist1 = b-det;
        float hitDist2 = b+det;

        if(hitDist2 > 0) {
            if(hitDist1 < 0) {
                if(range==0.0f || hitDist2<range) {
                    result.type = INSIDE_PRIMITIVE;
                    result.distance = hitDist2;
                    result.LPOINT = ray.origin + ray.direction*hitDist2;
                    result.intersectionLPOINTNormal = (result.LPOINT - position)/radius;
                    result.object = this;
                }
            }
            else {
                if(range==0.0f || hitDist1 < range) {
                    result.type = HIT;
                    result.distance = hitDist1;
                    result.LPOINT = ray.origin + ray.direction*hitDist1;
                    result.intersectionLPOINTNormal = (result.LPOINT - position)/radius;
                    result.object = this;
                }
            }
        }
    }
    else if(det==0) {
        float hitDist = b;
        if(hitDist>=0) {
            if(range==0.0f || hitDist<range) {
                result.type = HIT;
                result.distance = hitDist;
                result.LPOINT = ray.origin + ray.direction*hitDist;
                result.intersectionLPOINTNormal = (result.LPOINT - position)/radius;
                result.object = this;
            }
        }
    }

    return result;
}

Vector3 Sphere::MapToLocal(const Vector3 &vec) const {
    Vector3 local = vec;
    local -= position;
    local/=radius;
    return local;
}

QDebug operator<<(QDebug dbg, const Sphere &s) {
    dbg.nospace() << "( position"<<s.position<<", radius "<<s.radius<<")";
    return dbg.space();
}
