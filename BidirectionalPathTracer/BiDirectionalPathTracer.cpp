#include "BidirectionalPathTracer.h"

BidirectionalPathTracer::BidirectionalPathTracer()
{
}

LightIntensity BidirectionalPathTracer::CalculateLightIntensity(Scene *scene, const Ray &ray)
{
    LightIntenisty Li;

    std::vector<Node> eyePath;
    GeneratePath(eyePath, scene, ray, EYE_REFLECTIONS);

    std::vector<Node> lightPath;
    float Le;
    Ray lightRay; // float Le = GetRandomLightRay(lightRay);
    GeneratePath(lightPath, scene, lightRay, LIGHT_REFLECTIONS);

    for (int i = 0; i < eyePath.size(); i++)
    {
        for (int j = 0; j < lightPath.size(); j++)
        {
            if (IsVisible(scene, eyePath[i].intersectionResult.LPOINT, lightPath.intersectionResult[j].LPOINT))
            {
                Li += Le * EvalPath(scene, eyePath, i, lightPath, j) / WeightPath(i, j);
            }
        }
    }
}

float BidirectionalPathTracer::EvalPath(Scene *scene, std::vector eyePath, int i, std::vector lightPath, int j)
{

}

float BidirectionalPathTracer::WeightPath(int i, int j)
{
    return (float) (i + j);
}

bool BidirectionalPathTracer::FindIntersectionInScene(Scene *scene, const Ray &ray, const IntersectionResult &intersection)
{
    int objectId = -1;
    float closestDistToIntersection = FLT_MAX;

    for (int i = 0; i < scene->geometry.count(); i++)
    {
        IntersectionResult result = scene->geometry.at(i)->Intersects(ray);

        if (result.type != MISS)
        {
            if (closestDistToIntersection > result.distance)
            {
                closestDistToIntersection = result.distance;
                objectId = i;
                intersection = result;
            }
        }
    }

    return objectId != -1;
}

bool BidirectionalPathTracer::IsVisible (Scene *scene, Vector3 a, Vector3 b)
{
    Vector3 dir = b - a;
    float dist = dir.GetLength();
    dir.Normalize();
    Ray ray = Ray(a, dir);

    for (int i = 0; i < scene->geometry.count(); i++)
    {
        IntersectionResult result = scene->geometry.at(i)->Intersects(ray, dist);

        if (result.type != MISS)
        {
            return false;
        }
    }

    return true;
}

void BidirectionalPathTracer::GeneratePath(std::vector<Node> &path, Scene *scene, const Ray &rayIn, const int &maxReflections)
{
    IntersectionResult intersection;

    int reflections = 0;

     if (FindIntersectionInScene(scene, rayIn, intersection))
     {
         //////////////////////////////TODO////////////////////////////////
         float weight = 0.0f;
         Ray rayOut;
         //TODO Odbicie BRDF -> weight = BRDF(rayIn, intersection, rayOut)
         //////////////////////////////////////////////////////////////////
         path.push_back(Node(intersection, weight));

         while (reflections < maxReflections && FindIntersectionInScene(scene, rayOut, intersection))
         {
             //////////////////////////////TODO////////////////////////////////
             //TODO Odbicie BRDF -> weight = BRDF(rayIn, intersection, rayOut)
             //////////////////////////////////////////////////////////////////
             path.push_back(Node(intersection, weight));
             reflections++;
         }
     }

     return path;
}
