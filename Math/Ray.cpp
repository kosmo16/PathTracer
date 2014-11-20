#include <Math/Ray.h>

#include <Math/Plane.h>

Ray::Ray(void)
{
    origin = Vector3(0,0,0);
    direction = Vector3(0,0,1);
}

Ray::Ray(const Vector3 &orig, const Vector3 &dir) : origin(orig), direction(dir) {
    direction.Normalize();
}

QDebug operator<<(QDebug dbg, const Ray &r) {
    dbg.nospace() << "( origin" << r.origin <<", dir" << r.direction <<")";
    return dbg.space();
}


QDebug operator<<(QDebug dbg, const IntersectionResult &ir) {
    if(ir.type==MISS) {
        dbg.nospace() <<"no intersection";
    }
    else if(ir.type==HIT) {
        dbg.nospace() <<"intersection at LPOINT "<<ir.LPOINT<<", distance"<<ir.distance<<", normal at that LPOINT"<<ir.intersectionLPOINTNormal;
    }
    else {
        dbg.nospace() <<"intersection at LPOINT inside primitive "<<ir.LPOINT<<", distance "<<ir.distance<<", normal at that LPOINT"<<ir.intersectionLPOINTNormal;
    }

    return dbg.space();
}
