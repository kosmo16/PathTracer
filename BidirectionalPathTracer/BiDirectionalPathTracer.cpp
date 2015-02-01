#include "BidirectionalPathTracer.h"

const int BidirectionalPathTracer::EYE_REFLECTIONS;
const int BidirectionalPathTracer::LIGHT_REFLECTIONS;

BidirectionalPathTracer::BidirectionalPathTracer(Brdf * const brdf, Pdf * const pdf)
    : brdf(brdf),
      pdf(pdf)
{
}

Ray BidirectionalPathTracer::GetRandomLightRay(Scene *scene)
{
    QList<AmbientLight*> & lights = scene->lights;
    int numberOfLights = lights.size();
    AmbientLight* randomLight = lights[rand() % numberOfLights];
    Ray randomLightRay = randomLight->GetPhoton(false);
    return randomLightRay;
}

LightIntensity BidirectionalPathTracer::CalculateLightIntensity(Scene *scene, const Ray &ray, const Vector3 cameraPosition)
{
    LightIntensity Li;

    std::vector<Node> eyePath;
    GeneratePath(eyePath, scene, ray, EYE_REFLECTIONS);

    std::vector<Node> lightPath;
    float Le = 1.0f;
    Ray lightRay = GetRandomLightRay(scene);
    GeneratePath(lightPath, scene, lightRay, LIGHT_REFLECTIONS);

    for (int i = 0; i < eyePath.size(); i++)
    {
        Li += scene->lights.at(0)->GetLightIntensity(cameraPosition, &eyePath[i].intersectionResult, scene->geometry) / eyePath.size();

        if(eyePath[i].intersectionResult.object->GetMaterial()->texture) {
            Li *= eyePath[i].intersectionResult.object->GetMaterial()->texture->SampleSpherical(eyePath[i].intersectionResult.object->MapToLocal(eyePath[i].intersectionResult.LPOINT));
        }


        //        for (int j = 0; j < lightPath.size(); j++)
        //        {
        //            if (IsVisible(scene, eyePath[i].intersectionResult.LPOINT, lightPath[j].intersectionResult.LPOINT))
        //            {
        //                Li += Le * EvalPath(scene, eyePath, i, lightPath, j) / WeightPath(i, j);
        //            }
        //        }
    }

    return Li;
}

LightIntensity BidirectionalPathTracer::EvalPath(Scene *scene,
                                                 const std::vector<Node> &eyePath, int i,
                                                 const std::vector<Node> &lightPath, int j)
{
    LightIntensity L(1.0, 1.0, 1.0);

    for (int eyeNodeNumber = 0; eyeNodeNumber <= i; ++eyeNodeNumber) {
        const Node & eyeNode = eyePath[eyeNodeNumber];
        const IntersectionResult & intersection = eyeNode.intersectionResult;
        const Geometry * const & intersectionObject = intersection.object;
        L *= intersectionObject->GetMaterial()->texture->SampleSpherical(intersectionObject->MapToLocal(intersection.LPOINT));
    }

    // sciezka cienia
    const Node & eyeNode = eyePath[i];
    const Node & lightNode = lightPath[lightPath.size() - 1];
    bool isShadowVisible = IsVisible(scene, eyeNode.intersectionResult.LPOINT, lightNode.intersectionResult.LPOINT);
    if(!isShadowVisible)
    {
        return LightIntensity();
    }
    // L *= ;

    for (int lightNodeNumber = lightPath.size() - 1; lightNodeNumber >= j; --lightNodeNumber) {
        const Node & lightNode = lightPath[lightNodeNumber];
        const IntersectionResult & intersection = lightNode.intersectionResult;
        const Geometry * const & intersectionObject = intersection.object;
        L *= intersectionObject->GetMaterial()->texture->SampleSpherical(intersectionObject->MapToLocal(intersection.LPOINT));
    }

    return L;
}

float BidirectionalPathTracer::WeightPath(int i, int j)
{
    return (float) (i + j);
}

bool BidirectionalPathTracer::FindIntersectionInScene(Scene *scene, const Ray &ray, IntersectionResult &intersection)
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

bool BidirectionalPathTracer::IsVisible(Scene *scene, const Vector3 &a, const Vector3 &b)
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

std::vector<Node> BidirectionalPathTracer::GeneratePath(std::vector<Node> &path, Scene *scene, const Ray &rayIn, const int &maxReflections)
{
    IntersectionResult intersection;
    if (FindIntersectionInScene(scene, rayIn, intersection))
    {
        const Vector3 &origin = intersection.LPOINT;
        const Vector3 &normal = intersection.intersectionLPOINTNormal;
        const Vector3 &rayInDirection = rayIn.direction;

        int reflections = 0;
        do
        {
            Vector3 outDirection = pdf->computeDirection(rayInDirection, normal);
            Ray rayOut(origin, outDirection);
            float weight = brdf->computeRatio(rayInDirection, rayOut.direction, normal);
            path.push_back(Node(intersection, weight));
            path.push_back(Node(intersection, weight));
            reflections++;
        }
        while (reflections <= maxReflections && FindIntersectionInScene(scene, rayOut, intersection));
    }

    return path;
}
