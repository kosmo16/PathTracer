#include "BidirectionalPathTracer.h"

const int BidirectionalPathTracer::EYE_REFLECTIONS;
const int BidirectionalPathTracer::LIGHT_REFLECTIONS;

#define BIAS 0.001f

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

        const IntersectionResult &eyeIntersection = eyePath[i].intersectionResult;
        const Geometry * const &geometry = eyeIntersection.object;
        const Texture * const & texture = geometry->GetMaterial()->texture;

        if(texture) {
            Li *= texture->SampleSpherical(geometry->MapToLocal(eyeIntersection.LPOINT));
        }

        //        const IntersectionResult &lightIntersection = lightPath[i].intersectionResult;

        //        for (int j = 0; j < lightPath.size(); j++)
        //        {
        //            if (IsVisible(scene, eyeIntersection.LPOINT, lightIntersection.LPOINT))
        //            {
        //                Li += Le * EvalPath(scene, eyePath, i, lightPath, j) / WeightPath(i, j);
        //            }
        //        }
    }

    return Li;
}

void BidirectionalPathTracer::changeL(const Geometry* const &intersectionObject, LightIntensity &L, const IntersectionResult &intersection)
{
    const Vector3 &origin = intersection.LPOINT;
    const Material* const &material = intersectionObject->GetMaterial();

    /*
    const Vector3 &normal = intersection.intersectionLPOINTNormal;

    if(material->type == REFLECTIVE && reflections > 0) {
        Vector3 reflected = ray.direction.Reflect(normal);
        reflected.Normalize();
        Ray newRay(origin + reflected * BIAS, reflected);

        // resultIntensity += TraceRay(newRay, scene, cameraPosition, reflections - 1, exposure, globalMap, causticMap);
    }
    else if(material->type == REFRACTIVE && reflections > 0) {
        float reflectionCoef = max(0.0, min(1.0, 0.05 + 0.11 * (1 + ray.direction.DotProduct(normal))));
        LightIntensity reflectedIntensity;
        LightIntensity refractedIntensity;

        RefractiveMaterial* mat = (RefractiveMaterial*)material;

        Vector3 refracted;
        if(intersection.type == HIT)
            refracted = ray.direction.Refract(normal, mat->etaRate);
        else
            refracted = ray.direction.Refract(-normal, 1.0f / mat->etaRate);
        refracted.Normalize();

        Ray newRay(origin + refracted * BIAS, refracted);

        // refractedIntensity += TraceRay(newRay, scene, cameraPosition, reflections - 1, exposure, globalMap, causticMap);

        Vector3 reflected = ray.direction.Reflect(normal);
        reflected.Normalize();
        newRay = Ray(origin + reflected * BIAS, reflected);

        // reflectedIntensity += TraceRay(newRay, scene, cameraPosition, reflections - 1, exposure, globalMap, causticMap);

        // resultIntensity = reflectionCoef*reflectedIntensity + (1-reflectionCoef)*refractedIntensity;
    }
    */

    L *= material->texture->SampleSpherical(intersectionObject->MapToLocal(origin));
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

        changeL(intersectionObject, L, intersection);
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

        changeL(intersectionObject, L, intersection);
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

std::vector<Node>& BidirectionalPathTracer::GeneratePath(std::vector<Node> &path, Scene *scene, const Ray &rayIn, const int &maxReflections)
{
    IntersectionResult intersection;
    if (FindIntersectionInScene(scene, rayIn, intersection))
    {
        const Vector3 &origin = intersection.LPOINT;
        const Vector3 &normal = intersection.intersectionLPOINTNormal;
        const Vector3 &rayInDirection = rayIn.direction;

        int reflections = 0;
        bool intersectionInScene = false;
        do
        {
            Vector3 outDirection = pdf->computeDirection(rayInDirection, normal);
            Ray rayOut(origin, outDirection);
            float weight = brdf->computeRatio(rayInDirection, rayOut.direction, normal);
            path.push_back(Node(intersection, weight));
            reflections++;

            intersectionInScene = FindIntersectionInScene(scene, rayOut, intersection);
        }
        while (reflections <= maxReflections && intersectionInScene);
    }

    return path;
}
