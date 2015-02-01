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
        // L *= ;
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
        // L *= ;
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

//LightIntensity BidirectionalPathTracer::TracePath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections)
//{
//    for (int i = 0; i < scene->lights.count(); i++)
//    {

//    }
//}

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
    const Vector3 &origin = intersection.LPOINT;
    const Vector3 &normal = intersection.intersectionLPOINTNormal;
    const Vector3 &rayInDirection = rayIn.direction;

    int reflections = 0;

     if (FindIntersectionInScene(scene, rayIn, intersection))
     {
         Vector3 outDirection = pdf->computeDirection(rayInDirection, normal);
         Ray rayOut(origin, outDirection);
         float weight = brdf->computeRatio(rayInDirection, rayOut.direction, normal);
         path.push_back(Node(intersection, weight));
         path.push_back(Node(intersection, weight));

         while (reflections < maxReflections && FindIntersectionInScene(scene, rayOut, intersection))
         {
             Vector3 outDirection = pdf->computeDirection(rayInDirection, normal);
             Ray rayOut(origin, outDirection);
             float weight = brdf->computeRatio(rayInDirection, rayOut.direction, normal);
             path.push_back(Node(intersection, weight));
             path.push_back(Node(intersection, weight));
             reflections++;
         }
     }

     return path;
}

//void BidirectionalPathTracer::GetPath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections)
//{
//    LightIntensity resultIntensity;

//    const int remainingEyeReflections = maxReflections;
//    const int remainingLightReflections = maxReflections;
//    int eyeReflectionsCount = 0;

//    // Searching for intersection with any object if exists
//    int objectId = -1;
//    float closestDistToIntersection = FLT_MAX;
//    IntersectionResult closestIntersection;

//    for (int i = 0; i < scene->geometry.count(); i++)
//    {
//        IntersectionResult result = scene->geometry.at(i)->Intersects(ray);

//        if (result.type != MISS)
//        {
//            if (closestDistToIntersection > result.distance)
//            {
//                closestDistToIntersection = result.distance;
//                objectId = i;
//                closestIntersection = result;
//            }
//        }
//    }

//    if(objectId != -1) {
//        //        if(closestIntersection.object->GetMaterial()->type==REFLECTIVE && reflections>0) {
//        //            Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
//        //            reflected.Normalize();
//        //            Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);

//        //            resultIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);
//        //        }
//        //        else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE && reflections>0) {
//        //            float reflectionCoef = max(0.0, min(1.0, 0.05+0.11*(pow(1+ray.direction.DotProduct(closestIntersection.intersectionLPOINTNormal), 1))));
//        //            LightIntensity reflectedIntensity;
//        //            LightIntensity refractedIntensity;

//        //            RefractiveMaterial* mat = (RefractiveMaterial*)closestIntersection.object->GetMaterial();

//        //            Vector3 refracted;
//        //            if(closestIntersection.type==HIT)
//        //                refracted = ray.direction.Refract(closestIntersection.intersectionLPOINTNormal,
//        //                                                  mat->etaRate);
//        //            else
//        //                refracted = ray.direction.Refract(-closestIntersection.intersectionLPOINTNormal,
//        //                                                  1.0f/mat->etaRate);
//        //            refracted.Normalize();

//        //            Ray newRay(closestIntersection.LPOINT+refracted*BIAS, refracted);

//        //            refractedIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);

//        //            Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
//        //            reflected.Normalize();
//        //            newRay = Ray(closestIntersection.LPOINT+reflected*BIAS, reflected);

//        //            reflectedIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);


//        //            resultIntensity = reflectionCoef*reflectedIntensity + (1-reflectionCoef)*refractedIntensity;
//        //        }
//        //        else {
//        //            LightIntensity fromLights;
//        //            for(int j=0;j<scene->lights.count();j++) {
//        //                fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &closestIntersection, scene->geometry);
//        //            }
//        //            resultIntensity += fromLights;
//        //        }

//        //        if(closestIntersection.object->GetMaterial()->texture) {
//        //            resultIntensity*=closestIntersection.object->GetMaterial()->texture->SampleSpherical(closestIntersection.object->MapToLocal(closestIntersection.LPOINT));
//        //        }
//        //    }
//        //    return resultIntensity;
//    }
//}
