#include "BiDirectionalPathTracer.h"

BiDirectionalPathTracer::BiDirectionalPathTracer()
{
}

LightIntensity BiDirectionalPathTracer::TracePath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections)
{
    LightIntensity resultIntensity;

    int closest=-1;
    float closestDist=FLT_MAX;
    IntersectionResult closestIntersection;

    IntersectionResult result;
    for(int j=0;j<scene->geometry.count();j++) {
        result = scene->geometry.at(j)->Intersects(ray);
        if(result.type!=MISS) {
            if(closestDist>result.distance) {
                closestDist = result.distance;
                closest = j;
                closestIntersection = result;
            }
        }
    }

    if(closest!=-1) {
        if(closestIntersection.object->GetMaterial()->type==REFLECTIVE && reflections>0) {
            Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
            reflected.Normalize();
            Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);

            resultIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);
        }
        else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE && reflections>0) {
            float reflectionCoef = max(0.0, min(1.0, 0.05+0.11*(pow(1+ray.direction.DotProduct(closestIntersection.intersectionLPOINTNormal), 1))));
            LightIntensity reflectedIntensity;
            LightIntensity refractedIntensity;

            RefractiveMaterial* mat = (RefractiveMaterial*)closestIntersection.object->GetMaterial();

            Vector3 refracted;
            if(closestIntersection.type==HIT)
                refracted = ray.direction.Refract(closestIntersection.intersectionLPOINTNormal,
                                                  mat->etaRate);
            else
                refracted = ray.direction.Refract(-closestIntersection.intersectionLPOINTNormal,
                                                  1.0f/mat->etaRate);
            refracted.Normalize();

            Ray newRay(closestIntersection.LPOINT+refracted*BIAS, refracted);

            refractedIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);

            Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
            reflected.Normalize();
            newRay = Ray(closestIntersection.LPOINT+reflected*BIAS, reflected);

            reflectedIntensity += TraceRay(newRay, scene, cameraPosition, reflections-1, exposure, globalMap, causticMap);


            resultIntensity = reflectionCoef*reflectedIntensity + (1-reflectionCoef)*refractedIntensity;
        }
        else {
            LightIntensity fromLights;
            for(int j=0;j<scene->lights.count();j++) {
                fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &closestIntersection, scene->geometry);
            }
            resultIntensity += fromLights;

            if(globalMap) {
                QList<Photon*> closest = globalMap->GetClosestPhotons(closestIntersection.LPOINT, globalRadius, globalNumPhotons);

                if(closest.count()!=0) {
                    float r = (closest[0]->position-closestIntersection.LPOINT).GetLength();

                    LightIntensity E;
                    for(int z=0;z<closest.count();z++) {
                        float dot = closestIntersection.intersectionLPOINTNormal.DotProduct(closest[z]->direction);
                        if(dot>0) {
                            E += exposure*closest[z]->energy*dot;
                        }
                    }
                    resultIntensity += (E/(M_PI*r*r));
                }
            }

            if(causticMap) {
                QList<Photon*> closest = causticMap->GetClosestPhotons(closestIntersection.LPOINT, causticRadius, causticNumPhotons);

                if(closest.count()!=0) {
                    float r = (closest[0]->position-closestIntersection.LPOINT).GetLength();

                    LightIntensity E;
                    for(int z=0;z<closest.count();z++) {
                        float dot = closestIntersection.intersectionLPOINTNormal.DotProduct(closest[z]->direction);
                        if(dot>0) {
                            E += exposure*closest[z]->energy*dot;
                        }
                    }
                    resultIntensity += (E/(M_PI*r*r));
                }
            }

        }
        if(closestIntersection.object->GetMaterial()->texture) {
            resultIntensity*=closestIntersection.object->GetMaterial()->texture->SampleSpherical(closestIntersection.object->MapToLocal(closestIntersection.LPOINT));
        }
    }
    return resultIntensity;

}
