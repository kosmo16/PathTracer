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

LightIntensity BidirectionalPathTracer::getLightIntensity(const Vector3 &previousPosition,
                                                          const IntersectionResult &currentPosition,
                                                          const LightIntensity &incomingColor) const
{
    LightIntensity lightIntensity;

    return lightIntensity;
}

LightIntensity BidirectionalPathTracer::TracePath(const Ray &ray, Scene *scene, const Vector3 cameraPosition)
{
    std::vector<Node> eyePath;
    GeneratePath(eyePath, scene, ray, EYE_REFLECTIONS);

    std::vector<Node> lightPath;
    Ray lightRay;
    GeneratePath(lightPath, scene, lightRay, LIGHT_REFLECTIONS);
    LightIntensity resultIntensity;

    float Le = 1.0f; // powinno byc od swiatla attenuation

    for (std::vector<Node>::size_type i = 0; i < eyePath.size(); i++)
    {
        const Vector3 &eyePoint = eyePath[i].intersectionResult.LPOINT;
        for (std::vector<Node>::size_type j = 0; j < lightPath.size(); j++)
        {
            const Vector3 &lightPoint = lightPath[j].intersectionResult.LPOINT;
            if (IsVisible(scene, eyePoint, lightPoint))
            {
                float weightPath = WeightPath(i, j);
                if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::TracePath - path - weightPath: " << weightPath;
                if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::TracePath - path - resultIntensity: " << resultIntensity;
                try
                {
                    const LightIntensity evalPath = EvalPath(scene, eyePath, i, lightPath, j);
                    if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::TracePath - path - evalPath: " << evalPath;
                    resultIntensity += Le * evalPath / weightPath;
                    if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::TracePath - path - resultIntensity: " << resultIntensity;
                }
                catch(long error)
                {
                    if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer - blad: " << error;
                    if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer - i: " << i << ", j: " << j;
                    if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer - eyePoint: " << eyePoint << ", lightPoint: " << lightPoint;
                    throw '1';
                }
            }
        }

        //Direct ilumination
        LightIntensity fromLights;
        for (int j = 0; j < scene->lights.size(); j++)
        {
            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &eyePath[i].intersectionResult, scene->geometry);
            if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::TracePath - direct - fromLights: " << fromLights;
        }

        resultIntensity += fromLights;
    }

    static LightIntensity minIntensity(INFINITY, INFINITY, INFINITY);
    static LightIntensity maxIntensity(-INFINITY, -INFINITY, -INFINITY);
    bool change = false;
    if(minIntensity.r > resultIntensity.r) minIntensity.r = resultIntensity.r, change = true;
    if(minIntensity.g > resultIntensity.g) minIntensity.g = resultIntensity.g, change = true;
    if(minIntensity.b > resultIntensity.b) minIntensity.b = resultIntensity.b, change = true;
    if(maxIntensity.r < resultIntensity.r) maxIntensity.r = resultIntensity.r, change = true;
    if(maxIntensity.g < resultIntensity.g) maxIntensity.g = resultIntensity.g, change = true;
    if(maxIntensity.b < resultIntensity.b) maxIntensity.b = resultIntensity.b, change = true;
    static int counter = 0;
    if(++counter > 100000 && change)
    {
        qDebug() << __LINE__ << ". BidirectionalPathTracer::TracePath - min: " << minIntensity << ", max: " << maxIntensity << ", counter: " << counter;
    }

//    resultIntensity += 2.22452f;
//    resultIntensity *= 0.5f * M_1_PI;
    return resultIntensity;
}

AmbientLight* BidirectionalPathTracer::GetRandomLightRay(const Scene* const &scene, Ray &randomLightRay)
{
    const QList<AmbientLight*> & lights = scene->lights;
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
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::GetIntensity - true - intensity: " << intensity;
    }
    else
    {
        float weight = node.weight;
        const Color &color = ((DiffuseMaterial*)material)->diffuse;
        intensity = weight * color;
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::GetIntensity - false - weight: " << weight << ", color: " << color << ", intensity: " << intensity;
    }
    return intensity;
}

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
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::EvalPath - eyeNode - L: " << L;
    }

    for (int lightNodeNumber = lightPath.size() - 1; lightNodeNumber >= j; --lightNodeNumber)
    {
        const Node & lightNode = lightPath[lightNodeNumber];

        L *= GetIntensity(lightNode);
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::EvalPath - lightNode - L: " << L;
    }

    if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::EvalPath - L: " << L;
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

    if(intersection.intersectionLPOINTNormal.x != intersection.intersectionLPOINTNormal.x
            || intersection.intersectionLPOINTNormal.y != intersection.intersectionLPOINTNormal.y
            || intersection.intersectionLPOINTNormal.z != intersection.intersectionLPOINTNormal.z)
    {
        //        qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - NaN";
        //        qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - intersection: " << intersection;
        //        qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - objectId: " << objectId;
        //        qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - object: " << scene->geometry;
        //        throw 1l;
        return false;
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
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - 1";
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
            if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - 2";
            rayOut = new Ray(origin + refracted * BIAS, refracted);
            path.push_back(Node(intersection, (1.0-reflectionCoef)));
        }
        else
        {
            if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - 3";
            Vector3 reflected = rayInDirection.Reflect(normal);
            reflected.Normalize();
            rayOut = new Ray(origin + reflected * BIAS, reflected);
            path.push_back(Node(intersection, reflectionCoef));
        }
    }
    else
    {
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - 4";
        Vector3 outDirection = pdf->computeDirection(rayInDirection, normal);
        Ray rayOut(origin + outDirection * BIAS, outDirection);
        float weight = brdf->computeRatio(rayInDirection, rayOut.direction, normal);
        if(weight != weight)
        {
            if(DEBUGRR) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - rayInDirection: " << rayInDirection;
            if(DEBUGRR) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - normal: " << normal;
            if(DEBUGRR) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - outDirection: " << outDirection;
            if(DEBUGRR) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - rayOut: " << rayOut;
            if(DEBUGRR) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - weight: " << weight;
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
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::GeneratePath - normal: " << normal;
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::GeneratePath - rayInDirection: " << rayInDirection;

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
                qDebug() << __LINE__ << ". BidirectionalPathTracer::GeneratePath - error: " << error;
                qDebug() << __LINE__ << ". BidirectionalPathTracer::GeneratePath - intersection: " << intersection;
                qDebug() << __LINE__ << ". BidirectionalPathTracer::GeneratePath - rayIn: " << rayIn;
                qDebug() << __LINE__ << ". BidirectionalPathTracer::GeneratePath - normal: " << normal;
                qDebug() << __LINE__ << ". BidirectionalPathTracer::GeneratePath - rayInDirection: " << rayInDirection;
                throw '1';
            }
        }
        while (reflections <= maxReflections && intersectionInScene);
    }

    return path;
}

#undef DEBUG
#undef DEBUGRR
