#ifndef __PLANE_H__
#define __PLANE_H__

class Vector3;
#include "Ray.h"

const int LPOINT_ON_PLANE=0;
const int LPOINT_IN_FRONT_OF_PLANE=1;
const int LPOINT_BEHIND_PLANE=2;

const int EPSILON=0.00001;

class Plane
{
public:
    Vector3 normal;
    float distance;

    Plane():normal(Vector3(0.0f, 0.0f, 0.0f)), distance(0.0f) {}
    Plane(Vector3 newNormal, float newDistance):normal(newNormal), distance(newDistance) {}
	
    Plane(const Plane &plane) 	{
        normal=plane.normal;
        distance=plane.distance;
    }

    ~Plane() {}
	
    void SetNormal(const Vector3 &vec) {
        normal=vec;
    }
    void SetDistance(float newDistance) {
        distance=newDistance;
    }

    void SetFromLPOINTs(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2);

    void CalculateDistance(const Vector3 &LPOINTOnPlane) {
        distance=-normal.DotProduct(LPOINTOnPlane);
    }
	
    void Normalize(void);

    Vector3 GetNormal() { return normal; }
    float GetDistance() { return distance; }
	
    bool Intersect3(const Plane &p2, const Plane &p3, Vector3 &result);

    float GetDistanceFromLPOINT(const Vector3 &LPOINT) const;
    int ClassifyLPOINT(const Vector3 &LPOINT) const;

    Plane Lerp(const Plane &p2, float factor);

    bool operator==(const Plane &plane) const;
    bool operator!=(const Plane &plane) const {
        return!((*this)==plane);
    }

    Plane operator-(void) const {return Plane(-normal, -distance);}
    Plane operator+(void) const {return (*this);}

    IntersectionResult Intersects(const Ray& ray, float range=0.0f) const;
};



#endif
