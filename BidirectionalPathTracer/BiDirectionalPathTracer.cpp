#include "BidirectionalPathTracer.h"

const int BidirectionalPathTracer::EYE_REFLECTIONS;
const int BidirectionalPathTracer::LIGHT_REFLECTIONS;

#define BIAS 0.001f

#define DEBUG false
#define DEBUGRR true

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
    float Le = 1.0f; // powinno byc od swiatla atentuation

    for (std::vector<Node>::size_type i = 0; i < eyePath.size(); i++)
    {
        const Vector3 &eyePoint = eyePath[i].intersectionResult.LPOINT;
        for (std::vector<Node>::size_type j = 0; j < lightPath.size(); j++)
        {
            const Vector3 &lightPoint = lightPath[j].intersectionResult.LPOINT;
            if (IsVisible(scene, eyePoint, lightPoint))
            {
                float weightPath = WeightPath(i, j);
                if(DEBUG) qDebug() << "36. BidirectionalPathTracer::TracePath - path - weightPath: " << weightPath << "\n";
                if(DEBUG) qDebug() << "37. BidirectionalPathTracer::TracePath - path - resultIntensity: " << resultIntensity << "\n";
                try
                {
                    const LightIntensity evalPath = EvalPath(scene, eyePath, i, lightPath, j);
                    if(DEBUG) qDebug() << "41. BidirectionalPathTracer::TracePath - path - evalPath: " << evalPath;
                    resultIntensity += Le * evalPath / weightPath;
                    if(DEBUG) qDebug() << "43. BidirectionalPathTracer::TracePath - path - resultIntensity: " << resultIntensity;
                }
                catch(long error)
                {
                    if(DEBUG) qDebug() << "47. BidirectionalPathTracer - blad: " << error << "\n";
                    if(DEBUG) qDebug() << "48. BidirectionalPathTracer - i: " << i << ", j: " << j << "\n";
                    if(DEBUG) qDebug() << "49. BidirectionalPathTracer - eyePoint: " << eyePoint << ", lightPoint: " << lightPoint << "\n";
                    throw '1';
                }
            }
        }

        //Direct ilumination
        LightIntensity fromLights;
        for (int j = 0; j < scene->lights.size(); j++)
        {
            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &eyePath[i].intersectionResult, scene->geometry);
            if(DEBUG) qDebug() << "60. BidirectionalPathTracer::TracePath - direct - fromLights: " << fromLights;
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
        if(DEBUG) qDebug() << "86. BidirectionalPathTracer::GetIntensity - true - intensity: " << intensity;
    }
    else
    {
        float weight = node.weight;
        const Color &color = ((DiffuseMaterial*)material)->diffuse;
        intensity = weight * color;
        if(DEBUG) qDebug() << "93. BidirectionalPathTracer::GetIntensity - false - weight: " << weight << ", color: " << color << ", intensity: " << intensity;
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
        if(DEBUG) qDebug() << "164. BidirectionalPathTracer::EvalPath - eyeNode - L: " << L;
    }

    for (int lightNodeNumber = lightPath.size() - 1; lightNodeNumber >= j; --lightNodeNumber)
    {
        const Node & lightNode = lightPath[lightNodeNumber];

        L *= GetIntensity(lightNode);
        if(DEBUG) qDebug() << "172. BidirectionalPathTracer::EvalPath - lightNode - L: " << L;
    }

    if(DEBUG) qDebug() << "175. BidirectionalPathTracer::EvalPath - L: " << L;
    return L;
}

float BidirectionalPathTracer::WeightPath(int i, int j)
{
    return (float) (i + 2 + j);
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

    if(intersection.LPOINT.x != intersection.LPOINT.x
            || intersection.LPOINT.y != intersection.LPOINT.y
            || intersection.LPOINT.z != intersection.LPOINT.z)
    {
        qDebug() << "211. BidirectionalPathTracer::FindIntersectionInScene - NaN";
        qDebug() << "212. BidirectionalPathTracer::FindIntersectionInScene - intersection: " << intersection;
        qDebug() << "213. BidirectionalPathTracer::FindIntersectionInScene - objectId: " << objectId;
        qDebug() << "214. BidirectionalPathTracer::FindIntersectionInScene - object: " << scene->geometry;
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

Ray* BidirectionalPathTracer::RussianRoulette(const IntersectionResult &intersection,
                                              std::vector<Node> &path,
                                              const Vector3 &normal,
                                              const Vector3 &rayInDirection)
{
    Ray * rayOut;

    const Vector3 &origin = intersection.LPOINT;
    const Material* const &material = intersection.object->GetMaterial();

    if(material->type == REFLECTIVE)
    {
        if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - 1";
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
            if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - 2";
            rayOut = new Ray(origin + refracted * BIAS, refracted);
            path.push_back(Node(intersection, (1.0-reflectionCoef)));
        }
        else
        {
            if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - 3";
            Vector3 reflected = rayInDirection.Reflect(normal);
            reflected.Normalize();
            rayOut = new Ray(origin + reflected * BIAS, reflected);
            path.push_back(Node(intersection, reflectionCoef));
        }
    }
    else
    {
        if(DEBUG) qDebug() << "BidirectionalPathTracer::RussianRoulette - 4";
        Vector3 outDirection = pdf->computeDirection(rayInDirection, normal);
        Ray rayOut(origin + outDirection * BIAS, outDirection);
        float weight = brdf->computeRatio(rayInDirection, rayOut.direction, normal);
        if(weight != weight)
        {
            if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - rayInDirection: " << rayInDirection;
            if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - normal: " << normal;
            if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - outDirection: " << outDirection;
            if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - rayOut: " << rayOut;
            if(DEBUGRR) qDebug() << "BidirectionalPathTracer::RussianRoulette - weight: " << weight;
            throw 1l;
        }
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
        if(DEBUG) qDebug() << "306. BidirectionalPathTracer::GeneratePath - normal: " << normal;
        if(DEBUG) qDebug() << "307. BidirectionalPathTracer::GeneratePath - rayInDirection: " << rayInDirection;

        int reflections = 0;
        bool intersectionInScene = false;
        do
        {
            try
            {
                Ray * rayOut = RussianRoulette(intersection, path, normal, rayInDirection);

                reflections++;

                if (rayOut != NULL)
                {
                    intersectionInScene = FindIntersectionInScene(scene, *rayOut, intersection);
                }

                delete rayOut;
            }
            catch(long error)
            {
                qDebug() << "333. BidirectionalPathTracer::GeneratePath - error: " << error;
                qDebug() << "334. BidirectionalPathTracer::GeneratePath - intersection: " << intersection;
                qDebug() << "335. BidirectionalPathTracer::GeneratePath - rayIn: " << rayIn;
                qDebug() << "336. BidirectionalPathTracer::GeneratePath - normal: " << normal;
                qDebug() << "337. BidirectionalPathTracer::GeneratePath - rayInDirection: " << rayInDirection;
                throw '1';
            }
        }
        while (reflections <= maxReflections && intersectionInScene);
    }

    return path;
}

#undef DEBUG
#undef DEBUGRR
