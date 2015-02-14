#ifndef __NODE_H__
#define __NODE_H__

#include "RayTracer.h"

class Node
{
//private:
public:
    float brdfWeight;
    float relativeWeight;
    IntersectionResult intersectionResult;

public:
    Node(const IntersectionResult &intersectionResult, float brdfWeight, float relativeWeight);
    virtual ~Node();

    float getWeight() const;
};

#endif
