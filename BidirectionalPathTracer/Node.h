#ifndef NODE_H
#define NODE_H

#include "RayTracer.h"

class Node
{
//private:
public:
    float weight;
    IntersectionResult intersectionResult;

public:
    Node(float weight, const IntersectionResult &intersectionResult);
    Node(const IntersectionResult &intersectionResult, float weight);
    ~Node();
};

#endif // NODE_H
