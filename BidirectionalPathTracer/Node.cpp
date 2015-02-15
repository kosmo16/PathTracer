#include "Node.h"

Node::Node(const IntersectionResult &intersectionResult,
           float brdfWeight, float relativeWeight,
           /*const Vector3 &inDirection, const Vector3 &outDirection,*/
           LightIntensity intensity)
    : intersectionResult(intersectionResult),
      brdfWeight(brdfWeight),
      relativeWeight(relativeWeight),
      /*inDirection(inDirection),
      outDirection(outDirection),*/
      intensity(intensity)
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
