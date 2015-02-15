#include "pathtracer.h"

#define BIAS 0.001f



 int EYE_REFLECTIONS  = 4;
int LIGHT_REFLECTIONS = 4;

PathTracer::PathTracer()
{

}

PathTracer::~PathTracer()
{

}

AmbientLight* GetRandomLightRay( Scene* scene, Ray &randomLightRay)
{
    const QList<AmbientLight*> & lights = scene->lights;
    int numberOfLights = lights.size();
    AmbientLight* randomLight = lights[qrand() % numberOfLights];
    randomLightRay = randomLight->GetPhoton(false);
    return randomLight;
}

bool IsVisible( Scene* scene, const Vector3 &a, const Vector3 &b)
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

Vector3 UniformSampleHemisphere(const Vector3 &normal)
{
    float phi = randomUnsignedFloat(2.0f * M_PI);
    float rq = randomUnsignedFloat(1.0f);
    float r = sqrtf(rq);

    Vector3 V(cosf(phi) * r, sinf(phi) * r, sqrtf(1.0f - rq));
    V.Normalize();

    if (V.DotProduct(normal) < 0.0f)
    {
        V = -V;
    }
    return V;
}

bool PathTracer::FindIntersectionInScene(Scene* scene, const Ray &ray, IntersectionResult &intersection)
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

Ray PathTracer::CalculateNode(const IntersectionResult &closestIntersection, std::vector<Node> &path, const Vector3 &rayInDirection)
{
    if (closestIntersection.object->GetMaterial()->type==EMMISIVE)
    {
        EmmisiveMaterial* mat = (EmmisiveMaterial*)closestIntersection.object->GetMaterial();
        path.push_back(Node(closestIntersection, 0, 0, mat->light));
        return Ray();
    }
    else if(closestIntersection.object->GetMaterial()->type==REFLECTIVE) {
        Vector3 reflected = rayInDirection.Reflect(closestIntersection.intersectionLPOINTNormal);
        reflected.Normalize();
        path.push_back(Node(closestIntersection, 0, 0, LightIntensity(1.0, 1.0, 1.0)));
        return Ray(closestIntersection.LPOINT+reflected*BIAS, reflected);
    }
    else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE) {
        float reflectionCoef = max(0.0, min(1.0, 0.05+0.11*(pow(1.0+rayInDirection.DotProduct(closestIntersection.intersectionLPOINTNormal), 1))));
        LightIntensity reflectedIntensity;
        LightIntensity refractedIntensity;

        RefractiveMaterial* mat = (RefractiveMaterial*)closestIntersection.object->GetMaterial();

        Vector3 refracted;
        if(closestIntersection.type==HIT)
            refracted = rayInDirection.Refract(closestIntersection.intersectionLPOINTNormal, mat->etaRate);
        else
            refracted = rayInDirection.Refract(-closestIntersection.intersectionLPOINTNormal, 1.0f/mat->etaRate);
        refracted.Normalize();

        Ray newRayRefracted(closestIntersection.LPOINT+refracted*BIAS, refracted);
        path.push_back(Node(closestIntersection, 0, 0, LightIntensity(1, 1, 1)));

        Vector3 reflected = rayInDirection.Reflect(closestIntersection.intersectionLPOINTNormal);
        reflected.Normalize();
        Ray newRayReflected(closestIntersection.LPOINT+reflected*BIAS, reflected);

        if (randomFloat(0.0f, 1.0f) < reflectionCoef)
        {
            return newRayReflected;
        }
        else
        {
            return newRayRefracted;
        }
        //refractedIntensity += TracePath(newRay, scene, cameraPosition, n-1);



//        Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
//        reflected.Normalize();
//        newRay = Ray(closestIntersection.LPOINT+reflected*BIAS, reflected);

        //reflectedIntensity += TracePath(newRay, scene, cameraPosition, n-1);

        //resultIntensity = /*reflectionCoef*reflectedIntensity + */(1-reflectionCoef)*refractedIntensity;
    }
    else {
        DiffuseMaterial* mat = (DiffuseMaterial*)closestIntersection.object->GetMaterial();
        Vector3 N = closestIntersection.intersectionLPOINTNormal;
        Vector3 direction = UniformSampleHemisphere(N);
        direction.Normalize();
        Ray randomRay(closestIntersection.LPOINT + direction*BIAS, direction);
        float NdotRD = fabs(N.DotProduct(randomRay.direction));
        LightIntensity BRDF = mat->diffuse * (1.0f * NdotRD);
        //LightIntensity REFLECTED = TracePath(randomRay, scene, cameraPosition, n - 1);
        path.push_back(Node(closestIntersection, 0, 0, BRDF));
        //resultIntensity += BRDF * REFLECTED;
        return randomRay;
    }

//        LightIntensity fromLights;
//        for(int j=0;j<scene->lights.count();j++) {
//            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &closestIntersection, scene->geometry);
//        }
//        resultIntensity += 0.75 * fromLights;
 //   }
//    else
//    {
//        LightIntensity fromLights;
//        for(int j=0;j<scene->lights.count();j++) {
//            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &closestIntersection, scene->geometry);
//        }
//        resultIntensity += fromLights;
//    }
}

std::vector<Node>& PathTracer::GeneratePath(std::vector<Node> &path, Scene*scene, const Ray &firstRay, int maxReflections)
{
    Ray rayIn = firstRay;

    IntersectionResult intersection;
    bool intersectionInScene = FindIntersectionInScene(scene, rayIn, intersection);
    int reflections = 0;
    while (reflections <= maxReflections && intersectionInScene)
    {
        reflections++;

        Ray rayOut = CalculateNode(intersection, path, rayIn.direction);

        if (intersection.object->GetMaterial()->type == EMMISIVE)
        {
            intersectionInScene = false;
        }
        else
        {
            intersectionInScene = FindIntersectionInScene(scene, rayOut, intersection);
            rayIn = rayOut;
        }
    }

    return path;
}

LightIntensity PathTracer::EvalPath(std::vector<Node> &eyePath, int nEye, std::vector<Node> &lightPath, int nLight, Scene *scene)
{
    LightIntensity result(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < nEye - 1; i++)
    {
        result *= eyePath[i].intensity;
    }

    for (int i = 0; i < nLight - 1; i++)
    {
        result *= lightPath[i].intensity;
    }

    if (IsVisible(scene, eyePath[nEye - 1].intersectionResult.LPOINT, lightPath[nLight - 1].intersectionResult.LPOINT))
    {
        Vector3 out = eyePath[nEye - 1].intersectionResult.LPOINT - lightPath[nLight - 1].intersectionResult.LPOINT;
        out.Normalize();
        Vector3 out2 = -out;

        float NdotRD1 = fabs(lightPath[nLight - 1].intersectionResult.intersectionLPOINTNormal.DotProduct(out));

        if (lightPath[nLight - 1].intersectionResult.object->GetMaterial()->type == DIFFUSE)
        {
            // DiffuseMaterial* mat = (DiffuseMaterial*)lightPath[nLight - 1].intersectionResult.object->GetMaterial();
            // result *= mat->diffuse * (2.0f * NdotRD1);
            result *= (2.0f * NdotRD1);
        }
        else
        {
            return LightIntensity();
            //result *= (2.0f * NdotRD1);
        }

        float NdotRD2 = fabs(eyePath[nEye - 1].intersectionResult.intersectionLPOINTNormal.DotProduct(out2));

        if (eyePath[nEye - 1].intersectionResult.object->GetMaterial()->type == DIFFUSE)
        {
            DiffuseMaterial* mat = (DiffuseMaterial*)eyePath[nEye - 1].intersectionResult.object->GetMaterial();
            result *= mat->diffuse * (2.0f * NdotRD2);
        }
        else
        {
            return LightIntensity();
            //result *= (2.0f * NdotRD2);
        }

        return result;
    }
    else
    {
        return LightIntensity();
    }
}

LightIntensity PathTracer::TracePath(const Ray&ray, Scene*scene, const Vector3 cameraPosition, int n)
{
    std::vector<Node> eyePath;
    GeneratePath(eyePath, scene, ray, EYE_REFLECTIONS);

    std::vector<Node> lightPath;
    Ray lightRay;
    GetRandomLightRay(scene, lightRay);
    GeneratePath(lightPath, scene, lightRay, LIGHT_REFLECTIONS);

    LightIntensity result;

    if (eyePath.size() > 0 && eyePath[eyePath.size() - 1].intersectionResult.object->GetMaterial()->type == EMMISIVE)
    {
        result = eyePath[eyePath.size() - 1].intersectionResult.object->GetMaterial()->light;

        for (int i = 0; i < eyePath.size() - 1; i++)
        {
            result *= eyePath[i].intensity;
        }
    }
    else
    {
        for (int i = 0; i < eyePath.size(); i++)
        {
            for (int j = 0; j < lightPath.size(); j++)
            {
                LightIntensity partResult = EvalPath(eyePath, i + 1, lightPath, j + 1, scene);

                {
                    result += partResult / (i + j + 2) ;
                }
            }
        }
    }

    return result;
}
