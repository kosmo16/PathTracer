#include "Node.h"

Node::Node(float weight, const IntersectionResult &intersectionResult)
    : weight(weight), intersectionResult(intersectionResult)
{
    if(weight != weight)
    {
        qDebug() << "Node::Node w ir - weight: " << weight << ", intersectionResult: " << intersectionResult;
    }
}

Node::Node(const IntersectionResult &intersectionResult, float weight)
    : weight(weight), intersectionResult(intersectionResult)
{
    if(weight != weight)
    {
        qDebug() << "Node::Node ir w - weight: " << weight << ", intersectionResult: " << intersectionResult;
    }
}

Node::~Node()
{
}
