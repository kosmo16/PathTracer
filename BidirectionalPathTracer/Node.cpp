#include "Node.h"

Node::Node(const IntersectionResult &intersectionResult, float brdfWeight, float relativeWeight)
    : brdfWeight(brdfWeight), relativeWeight(relativeWeight), intersectionResult(intersectionResult)
{
    if(brdfWeight != brdfWeight)
    {
        qDebug() << __LINE__ << ". Node::Node w ir - brdfWeight: " << brdfWeight << ", intersectionResult: " << intersectionResult;
    }
    if(relativeWeight != relativeWeight)
    {
        qDebug() << __LINE__ << ". Node::Node w ir - relativeWeight: " << relativeWeight << ", intersectionResult: " << intersectionResult;
    }
}

Node::~Node()
{
}

float Node::getWeight() const
{
    return brdfWeight * relativeWeight;
}
