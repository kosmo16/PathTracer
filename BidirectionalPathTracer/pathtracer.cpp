#include "pathtracer.h"

#define BIAS 0.001f

int EYE_REFLECTIONS   = 3;
int LIGHT_REFLECTIONS = 0;

PathTracer::PathTracer()
{
}

PathTracer::~PathTracer()
{
}

AmbientLight* GetRandomLightRay(Scene* scene, Ray &randomLightRay)
{
    const QList<AmbientLight*> & lights = scene->lights;
    int numberOfLights = lights.size();
    AmbientLight* randomLight = lights[qrand() % numberOfLights];
    randomLightRay = randomLight->GetPhoton(false);
    return randomLight;
}

bool IsVisible(Scene* scene, const Vector3 &a, const Vector3 &b)
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

Vector3 UniformSampleHemisphere(Vector3 normal)
{
    float x, y, z;
    do {
        x = 2.0f*((float)rand()) / RAND_MAX - 1.0f;
        y = 2.0f*((float)rand()) / RAND_MAX - 1.0f;
        z = 2.0f*((float)rand()) / RAND_MAX - 1.0f;
    } while (x*x + y*y + z*z > 1);

    Vector3 direction(x, y, z);
    direction.Normalize();

    if (direction.DotProduct(normal) < 0)
        direction *= -1;

    return direction;
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

Ray PathTracer::CalculateNode(const IntersectionResult &closestIntersection, std::vector<Node> &path, const Vector3 &normal, const Vector3 &rayInDirection)
{
    if(closestIntersection.object->GetMaterial()->type==REFLECTIVE) {
        Vector3 reflected = rayInDirection.Reflect(closestIntersection.intersectionLPOINTNormal);
        reflected.Normalize();
        path.push_back(Node(closestIntersection, 0, 0, LightIntensity(1.0, 1.0, 1.0)));
        return Ray(closestIntersection.LPOINT+reflected*BIAS, reflected);
    }
    else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE) {
        float reflectionCoef = max(0.0, min(1.0, 0.05+0.11*(pow(1+rayInDirection.DotProduct(closestIntersection.intersectionLPOINTNormal), 1))));
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
        LightIntensity BRDF = mat->diffuse * (2.0f * NdotRD);
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

std::vector<Node>& PathTracer::GeneratePath(std::vector<Node> &path, Scene* scene, const Ray &rayIn, int &maxReflections)
{
    IntersectionResult intersection;

    if (FindIntersectionInScene(scene, rayIn, intersection))
    {
        // const Vector3 &origin = intersection.LPOINT;
        const Vector3 &normal = intersection.intersectionLPOINTNormal;
        const Vector3 &rayInDirection = rayIn.direction;
        int reflections = 0;

        bool intersectionInScene = false;
        do
        {
            reflections++;

            Ray rayOut = CalculateNode(intersection, path, normal, rayInDirection);
            intersectionInScene = FindIntersectionInScene(scene, rayOut, intersection);
        }
        while (reflections <= maxReflections && intersectionInScene);
    }

    return path;
}

LightIntensity PathTracer::TracePath(const Ray &ray, Scene* scene, const Vector3 cameraPosition, int n)
{
    std::vector<Node> eyePath;
    GeneratePath(eyePath, scene, ray, EYE_REFLECTIONS);

    std::vector<Node> lightPath;
    Ray lightRay;
    GetRandomLightRay(scene, lightRay);
    GeneratePath(lightPath, scene, lightRay, LIGHT_REFLECTIONS);

    LightIntensity result(1.0f, 1.0f, 1.0f);

    if (eyePath.size() == 0 || lightPath.size() == 0)
    {
        return LightIntensity();
    }

    for (int i = 0; i < eyePath.size() - 1; i++)
    {
        result *= eyePath[i].intensity;
    }

    for (int i = 0; i < lightPath.size() - 1; i++)
    {
        result *= lightPath[i].intensity;
    }

    if (IsVisible(scene, eyePath[eyePath.size() - 1].intersectionResult.LPOINT, lightPath[lightPath.size() - 1].intersectionResult.LPOINT))
    {
        Vector3 out = eyePath[eyePath.size() - 1].intersectionResult.LPOINT - lightPath[lightPath.size() - 1].intersectionResult.LPOINT;
        out.Normalize();
        Vector3 out2 = -out;

        float NdotRD1 = fabs(lightPath[lightPath.size() - 1].intersectionResult.intersectionLPOINTNormal.DotProduct(out));


        if (lightPath[lightPath.size() - 1].intersectionResult.object->GetMaterial()->type == DIFFUSE)
        {
            DiffuseMaterial* mat = (DiffuseMaterial*)lightPath[lightPath.size() - 1].intersectionResult.object->GetMaterial();
            result *= mat->diffuse * (2.0f * NdotRD1);
        }
        else
        {
            result *= (2.0f * NdotRD1);
        }

        float NdotRD2 = fabs(eyePath[eyePath.size() - 1].intersectionResult.intersectionLPOINTNormal.DotProduct(out2));

        if (eyePath[eyePath.size() - 1].intersectionResult.object->GetMaterial()->type == DIFFUSE)
        {
            DiffuseMaterial* mat = (DiffuseMaterial*)eyePath[eyePath.size() - 1].intersectionResult.object->GetMaterial();
            result *= mat->diffuse * (2.0f * NdotRD2);
        }
        else
        {
            result *= (2.0f * NdotRD2);
        }
    }
    else
    {
        return LightIntensity();
    }


//    int closest=-1;
//    float closestDist=FLT_MAX;
//    IntersectionResult closestIntersection;

//    for(int j=0;j<scene->geometry.count();j++) {
//        IntersectionResult result = scene->geometry.at(j)->Intersects(ray);
//        if(result.type!=MISS) {
//            if(closestDist>result.distance) {
//                closestDist = result.distance;
//                closest = j;
//                closestIntersection = result;
//            }
//        }
//    }

//    LightIntensity resultIntensity;

//    if(closest == -1) {
//        return resultIntensity;
//    }

//    if(closestIntersection.object->GetMaterial()->type==REFLECTIVE && n>0) {
//        Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
//        reflected.Normalize();
//        Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);

//        resultIntensity += TracePath(newRay, scene, cameraPosition, n - 1);
//    }
//    else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE && n>0) {
//        float reflectionCoef = max(0.0, min(1.0, 0.05+0.11*(pow(1+ray.direction.DotProduct(closestIntersection.intersectionLPOINTNormal), 1))));
//        LightIntensity reflectedIntensity;
//        LightIntensity refractedIntensity;

//        RefractiveMaterial* mat = (RefractiveMaterial*)closestIntersection.object->GetMaterial();

//        Vector3 refracted;
//        if(closestIntersection.type==HIT)
//            refracted = ray.direction.Refract(closestIntersection.intersectionLPOINTNormal, mat->etaRate);
//        else
//            refracted = ray.direction.Refract(-closestIntersection.intersectionLPOINTNormal, 1.0f/mat->etaRate);
//        refracted.Normalize();

//        Ray newRay(closestIntersection.LPOINT+refracted*BIAS, refracted);

//        refractedIntensity += TracePath(newRay, scene, cameraPosition, n-1);

//        Vector3 reflected = ray.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
//        reflected.Normalize();
//        newRay = Ray(closestIntersection.LPOINT+reflected*BIAS, reflected);

//        //reflectedIntensity += TracePath(newRay, scene, cameraPosition, n-1);

//        resultIntensity = /*reflectionCoef*reflectedIntensity + */(1-reflectionCoef)*refractedIntensity;
//    }
//    else if(n > 0) {
//        DiffuseMaterial* mat = (DiffuseMaterial*)closestIntersection.object->GetMaterial();
//        Vector3 N = closestIntersection.intersectionLPOINTNormal;
//        Ray randomRay(closestIntersection.LPOINT, UniformSampleHemisphere(N));
//        float NdotRD = fabs(N.DotProduct(randomRay.direction));
//        LightIntensity BRDF = mat->diffuse * (2.0f * NdotRD);
//        LightIntensity REFLECTED = TracePath(randomRay, scene, cameraPosition, n - 1);
//        resultIntensity += BRDF * REFLECTED;


////        LightIntensity fromLights;
////        for(int j=0;j<scene->lights.count();j++) {
////            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &closestIntersection, scene->geometry);
////        }
////        resultIntensity += 0.75 * fromLights;
//    }
//    else
//    {
//        return LightIntensity(1.0, 1.0, 1.0);
//    }
//    else
//    {
//        LightIntensity fromLights;
//        for(int j=0;j<scene->lights.count();j++) {
//            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &closestIntersection, scene->geometry);
//        }
//        resultIntensity += fromLights;
//    }

    return result;
}
