#include "BiDirectionalPathTracer.h"

BiDirectionalPathTracer::BiDirectionalPathTracer()
{
}

LightIntensity BiDirectionalPathTracer::TracePath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections)
{
    for (int i = 0; i < scene->lights.count(); i++)
    {

    }
}

void BiDirectionalPathTracer::GetPath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections)
{
    LightIntensity resultIntensity;

    const int remainingEyeReflections = maxReflections;
    const int remainingLightReflections = maxReflections;
    int eyeReflectionsCount = 0;

    // Searching for intersection with any object if exists
    int objectId = -1;
    float closestDistToIntersection = FLT_MAX;
    IntersectionResult closestIntersection;


    for (int i = 0; i < scene->geometry.count(); i++)
    {
        IntersectionResult result = scene->geometry.at(i)->Intersects(ray);

        if (result.type != MISS)
        {
            if (closestDistToIntersection > result.distance)
            {
                closestDistToIntersection = result.distance;
                objectId = j;
                closestIntersection = result;
            }
        }
    }

    if(objectId != -1) {
        //        if(closestIntersection.object->GetMaterial()->type==REFLECTIVE && reflections>0) {
        //            Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
        //            reflected.Normalize();
        //            Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);

        //            resultIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);
        //        }
        //        else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE && reflections>0) {
        //            float reflectionCoef = max(0.0, min(1.0, 0.05+0.11*(pow(1+ray.direction.DotProduct(closestIntersection.intersectionLPOINTNormal), 1))));
        //            LightIntensity reflectedIntensity;
        //            LightIntensity refractedIntensity;

        //            RefractiveMaterial* mat = (RefractiveMaterial*)closestIntersection.object->GetMaterial();

        //            Vector3 refracted;
        //            if(closestIntersection.type==HIT)
        //                refracted = ray.direction.Refract(closestIntersection.intersectionLPOINTNormal,
        //                                                  mat->etaRate);
        //            else
        //                refracted = ray.direction.Refract(-closestIntersection.intersectionLPOINTNormal,
        //                                                  1.0f/mat->etaRate);
        //            refracted.Normalize();

        //            Ray newRay(closestIntersection.LPOINT+refracted*BIAS, refracted);

        //            refractedIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);

        //            Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
        //            reflected.Normalize();
        //            newRay = Ray(closestIntersection.LPOINT+reflected*BIAS, reflected);

        //            reflectedIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);


        //            resultIntensity = reflectionCoef*reflectedIntensity + (1-reflectionCoef)*refractedIntensity;
        //        }
        //        else {
        //            LightIntensity fromLights;
        //            for(int j=0;j<scene->lights.count();j++) {
        //                fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &closestIntersection, scene->geometry);
        //            }
        //            resultIntensity += fromLights;
        //        }

        //        if(closestIntersection.object->GetMaterial()->texture) {
        //            resultIntensity*=closestIntersection.object->GetMaterial()->texture->SampleSpherical(closestIntersection.object->MapToLocal(closestIntersection.LPOINT));
        //        }
        //    }
        //    return resultIntensity;
    }
}
