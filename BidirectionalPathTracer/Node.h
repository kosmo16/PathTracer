#ifndef __NODE_H__
#define __NODE_H__

#include "RayTracer.h"

class Node
{
//private:
public:
    IntersectionResult intersectionResult;
    float brdfWeight;
    float relativeWeight;
    Vector3 inDirection;
    Vector3 outDirection;

public:
    Node(const IntersectionResult &intersectionResult,
         float brdfWeight, float relativeWeight,
         const Vector3 &inDirection, const Vector3 &outDirection);
    virtual ~Node();

    float getWeight() const;
};

#endif
