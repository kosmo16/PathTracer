#ifndef NODE_H
#define NODE_H

#include "RayTracer.h"

class Node
{
private:
    float weight;
    IntersectionResult intersectionResult;

public:
    Node(const float &weight, IntersectionResult intersectionResult);
    ~Node();
};

#endif // NODE_H
