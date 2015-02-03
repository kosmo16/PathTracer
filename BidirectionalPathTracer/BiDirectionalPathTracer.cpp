#include "BidirectionalPathTracer.h"

const int BidirectionalPathTracer::EYE_REFLECTIONS;
const int BidirectionalPathTracer::LIGHT_REFLECTIONS;

#define BIAS 0.001f

BidirectionalPathTracer::BidirectionalPathTracer(Brdf * const brdf, Pdf * const pdf)
    : brdf(brdf),
      pdf(pdf)
{
}

LightIntensity BidirectionalPathTracer::TracePath(const Ray &ray, Scene *scene, const Vector3 cameraPosition)
{
    LightIntensity resultIntensity;

    std::vector<Node> eyePath;
    GeneratePath(eyePath, scene, ray, EYE_REFLECTIONS);

    std::vector<Node> lightPath;
    Ray lightRay;
    // AmbientLight* light = GetRandomLightRay(scene, lightRay);
    GeneratePath(lightPath, scene, lightRay, LIGHT_REFLECTIONS);
    double Le = 1.0; // powinno byc od swiatla atentuation

    for (std::vector<Node>::size_type i = 0; i < eyePath.size(); i++)
    {
        for (std::vector<Node>::size_type j = 0; j < lightPath.size(); j++)
        {
            if (IsVisible(scene, eyePath[i].intersectionResult.LPOINT, lightPath[j].intersectionResult.LPOINT))
            {
                resultIntensity += Le * EvalPath(scene, eyePath, i, lightPath, j) / WeightPath(i, j);
                cout << "TracePath - path - resultIntensity: " << resultIntensity << "\n";
            }
        }

        //Direct ilumination
        LightIntensity fromLights;
        for (int j = 0; j < scene->lights.size(); j++)
        {
            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &eyePath[i].intersectionResult, scene->geometry);
            cout << "TracePath - direct - fromLights: " << fromLights << "\n";
        }

        resultIntensity += fromLights;
    }

    return resultIntensity;
}

AmbientLight* BidirectionalPathTracer::GetRandomLightRay(Scene *scene, Ray &randomLightRay)
{
    QList<AmbientLight*> & lights = scene->lights;
    int numberOfLights = lights.size();
    AmbientLight* randomLight = lights[rand() % numberOfLights];
    randomLightRay = randomLight->GetPhoton(false);
    return randomLight;
}

LightIntensity BidirectionalPathTracer::GetIntensity(const Node &node)
{
    LightIntensity intensity;
    const Material* const &material = node.intersectionResult.object->GetMaterial();
    const MaterialType& type = material->type;
    if (type == REFRACTIVE || type == REFLECTIVE)
    {
        intensity = node.weight * LightIntensity(1.0, 1.0, 1.0);
        cout << "GetIntensity - true - intensity: " << intensity << "\n";
    }
    else
    {
        intensity = node.weight * ((DiffuseMaterial*)material)->diffuse;
        cout << "GetIntensity - false - intensity: " << intensity << "\n";
    }
    return intensity;
}

//void BidirectionalPathTracer::changeL(const Geometry* const &intersectionObject, LightIntensity &L, const IntersectionResult &intersection)
//{
//    const Vector3 &origin = intersection.LPOINT;
//    const Material* const &material = intersectionObject->GetMaterial();

//    /*
//    const Vector3 &normal = intersection.intersectionLPOINTNormal;

//    if(material->type == REFLECTIVE && reflections > 0) {
//        Vector3 reflected = ray.direction.Reflect(normal);
//        reflected.Normalize();
//        Ray newRay(origin + reflected * BIAS, reflected);

//        // resultIntensity += TraceRay(newRay, scene, cameraPosition, reflections - 1, exposure, globalMap, causticMap);
//    }
//    else if(material->type == REFRACTIVE && reflections > 0) {
//        float reflectionCoef = max(0.0, min(1.0, 0.05 + 0.11 * (1 + ray.direction.DotProduct(normal))));
//        LightIntensity reflectedIntensity;
//        LightIntensity refractedIntensity;

//        RefractiveMaterial* mat = (RefractiveMaterial*)material;

//        Vector3 refracted;
//        if(intersection.type == HIT)
//            refracted = ray.direction.Refract(normal, mat->etaRate);
//        else
//            refracted = ray.direction.Refract(-normal, 1.0f / mat->etaRate);
//        refracted.Normalize();

//        Ray newRay(origin + refracted * BIAS, refracted);

//        // refractedIntensity += TraceRay(newRay, scene, cameraPosition, reflections - 1, exposure, globalMap, causticMap);

//        Vector3 reflected = ray.direction.Reflect(normal);
//        reflected.Normalize();
//        newRay = Ray(origin + reflected * BIAS, reflected);

//        // reflectedIntensity += TraceRay(newRay, scene, cameraPosition, reflections - 1, exposure, globalMap, causticMap);

//        // resultIntensity = reflectionCoef*reflectedIntensity + (1-reflectionCoef)*refractedIntensity;
//    }
//    */

//    L *= material->texture->SampleSpherical(intersectionObject->MapToLocal(origin));
//}

LightIntensity BidirectionalPathTracer::EvalPath(Scene *scene,
                                                 const std::vector<Node> &eyePath, int i,
                                                 const std::vector<Node> &lightPath, int j)
{
    LightIntensity L(1.0, 1.0, 1.0);

    // sciezka cienia
    const Node & eyeNode = eyePath[i];
    const Node & lightNode = lightPath[lightPath.size() - 1];
    bool isShadowVisible = IsVisible(scene, eyeNode.intersectionResult.LPOINT, lightNode.intersectionResult.LPOINT);
    if(!isShadowVisible)
    {
        return LightIntensity();
    }

    for (int eyeNodeNumber = 0; eyeNodeNumber <= i; ++eyeNodeNumber)
    {
        const Node & eyeNode = eyePath[eyeNodeNumber];

        L *= GetIntensity(eyeNode);
        cout << "EvalPath - eyeNode - L: " << L << "\n";
    }

    for (int lightNodeNumber = lightPath.size() - 1; lightNodeNumber >= j; --lightNodeNumber)
    {
        const Node & lightNode = lightPath[lightNodeNumber];

        L *= GetIntensity(lightNode);
        cout << "EvalPath - lightNode - L: " << L << "\n";
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

Ray *BidirectionalPathTracer::RussianRoulette(IntersectionResult intersection, std::vector<Node> &path, const Vector3 &normal, const Vector3 &rayInDirection)
{
    Ray * rayOut;

    const Vector3 &origin = intersection.LPOINT;
    const Material* const &material = intersection.object->GetMaterial();

    if(material->type == REFLECTIVE)
    {
        Vector3 reflected = rayInDirection.Reflect(normal);
        reflected.Normalize();
        rayOut = new Ray(origin + reflected * BIAS, reflected);
        path.push_back(Node(intersection, 1.0));
    }
    else if(material->type == REFRACTIVE)
    {
        double reflectionCoef = std::max(0.0, std::min(1.0, 0.05 + 0.11 * (1 + rayInDirection.DotProduct(normal))));

        RefractiveMaterial* mat = (RefractiveMaterial*)material;

        Vector3 refracted;

        if(intersection.type == HIT)
        {
            refracted = rayInDirection.Refract(normal, mat->etaRate);
        }
        else
        {
            refracted = rayInDirection.Refract(-normal, 1.0f / mat->etaRate);
        }

        refracted.Normalize();

        if (rand() > RAND_MAX / 2)
        {

            rayOut = new Ray(origin + refracted * BIAS, refracted);
            path.push_back(Node(intersection, (1.0-reflectionCoef)));
        }
        else
        {
            Vector3 reflected = rayInDirection.Reflect(normal);
            reflected.Normalize();
            rayOut = new Ray(origin + reflected * BIAS, reflected);
            path.push_back(Node(intersection, reflectionCoef));
        }
    }
    else
    {
        Vector3 outDirection = pdf->computeDirection(rayInDirection, normal);
        Ray rayOut(origin +  outDirection * BIAS, outDirection);
        float weight = brdf->computeRatio(rayInDirection, rayOut.direction, normal);
        path.push_back(Node(intersection, weight));
    }

    return rayOut;
}

std::vector<Node>& BidirectionalPathTracer::GeneratePath(std::vector<Node> &path, Scene *scene, const Ray &rayIn, const int &maxReflections)
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
            Ray * rayOut = RussianRoulette(intersection, path, normal, rayInDirection);
            reflections++;

            if (rayOut != NULL)
            {
                intersectionInScene = FindIntersectionInScene(scene, *rayOut, intersection);
            }
        }
        while (reflections <= maxReflections && intersectionInScene);
    }

    return path;
}
