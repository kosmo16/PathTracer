#include "Node.h"

Node::Node(const IntersectionResult &intersectionResult,
           float brdfWeight, float relativeWeight,
           LightIntensity intensity)///const Vector3 &inDirection, const Vector3 &outDirection)
    : intersectionResult(intersectionResult),
      brdfWeight(brdfWeight),
      relativeWeight(relativeWeight),
      inDirection(inDirection),
      outDirection(outDirection)
{
    this->intensity = intensity;

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
