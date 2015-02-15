#include "BidirectionalPathTracer.h"

const int BidirectionalPathTracer::EYE_REFLECTIONS;
const int BidirectionalPathTracer::LIGHT_REFLECTIONS;

#define BIAS 0.001f

#define DEBUG false
#define DEBUGRR true

BidirectionalPathTracer::BidirectionalPathTracer(const Brdf* const &brdf, const Pdf* const &pdf)
    : brdf(brdf),
      pdf(pdf)
{
}

LightIntensity BidirectionalPathTracer::TracePath(const Ray &ray, const Scene* const &scene, const Vector3 &cameraPosition) const
{
    std::vector<Node> eyePath;
    GeneratePath(eyePath, scene, ray, EYE_REFLECTIONS);

    std::vector<Node> lightPath;
    Ray lightRay;
    AmbientLight * light = GetRandomLightRay(scene, lightRay);
    GeneratePath(lightPath, scene, lightRay, LIGHT_REFLECTIONS);

    LightIntensity resultIntensity;

    for (std::vector<Node>::size_type i = 1; i <= eyePath.size(); i++)
    {
        const Vector3 &eyePoint = eyePath[i - 1].intersectionResult.LPOINT;

        for (std::vector<Node>::size_type j = 1; j <= lightPath.size(); j++)
        {
            const Vector3 &lightPoint = lightPath[j - 1].intersectionResult.LPOINT;

            if (IsVisible(scene, eyePoint, lightPoint))
            {
                float weightPath = WeightPath(i, j);
                LightIntensity evalPath = EvalPath(scene, eyePath, i, lightPath, j);
                resultIntensity += evalPath / weightPath;
            }
        }

        //Direct ilumination
        //        LightIntensity fromLights;
        //        for (int j = 0; j < scene->lights.size(); j++)
        //        {
        //            fromLights += scene->lights.at(j)->GetLightIntensity(cameraPosition, &eyePath[i].intersectionResult, scene->geometry);
        //            if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::TracePath - direct - fromLights: " << fromLights;
        //        }

        //resultIntensity += fromLights;
    }

    return resultIntensity;
}

AmbientLight* BidirectionalPathTracer::GetRandomLightRay(const Scene* const &scene, Ray &randomLightRay) const
{
    const QList<AmbientLight*> & lights = scene->lights;
    int numberOfLights = lights.size();
    AmbientLight* randomLight = lights[qrand() % numberOfLights];
    randomLightRay = randomLight->GetPhoton(false);
    return randomLight;
}

LightIntensity BidirectionalPathTracer::EvalPath(const Scene* const &scene,
                                                 const std::vector<Node> &eye, int nEye,
                                                 const std::vector<Node> &light, int nLight) const
{
    LightIntensity L(1.0, 1.0, 1.0);

    for (int i = 0; i < nEye - 1; ++i)
    {
        // Mnozenie * fabs(Dot(eye[i].dirOut, eye[i].normalGeometric)) *  ??eye[i].material->f(eye[i].dirIn, eye[i].dirOut)??
        //L *= GetIntensity(eye[i]) / (eye[i].brdfWeight * eye[i].relativeWeight);
        L *= GetIntensity(eye[i]) * /*(eye[i].brdfWeight) **/ fabs(eye[i].intersectionResult.intersectionLPOINTNormal.DotProduct(eye[i].outDirection));
    }

    Vector3 w = light[nLight-1].intersectionResult.LPOINT - eye[nEye-1].intersectionResult.LPOINT;

    //Vec3f ww = -w;
    //DOT
    //L *= GetIntensity(eye[nEye - 1]) / (eye[nEye-1].relativeWeight * light[nLight-1].relativeWeight);
    //L *= GetIntensity(eye[nEye - 1]);// * (0.2*0.2) ;

    for (int i = nLight-2; i >= 0; --i)
    {
        //L *= GetIntensity(light[i]) / (light[i].brdfWeight * light[i].relativeWeight);
        L *= GetIntensity(light[i]) */* (light[i].brdfWeight) **/ fabs(light[i].intersectionResult.intersectionLPOINTNormal.DotProduct(light[i].outDirection));
    }

    return L;
}


LightIntensity BidirectionalPathTracer::GetIntensity(const Node &node) const
{
    const Material* const &material = node.intersectionResult.object->GetMaterial();
    const MaterialType& type = material->type;
    if (type == REFRACTIVE || type == REFLECTIVE)
    {
        return LightIntensity(1.0, 1.0, 1.0);
        //if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::GetIntensity - true - intensity: " << intensity;
    }
    else
    {
        return ((DiffuseMaterial*)material)->diffuse;
        // if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::GetIntensity - false - weight: " << weight << ", color: " << color << ", intensity: " << intensity;
    }
}

//LightIntensity BidirectionalPathTracer::EvalPath(const Scene* const &scene,
//                                                 const std::vector<Node> &eyePath, int i,
//                                                 const std::vector<Node> &lightPath, int j,
//                                                 AmbientLight* light, const Vector3 &cameraPosition) const
//{
//    LightIntensity L(1.0, 1.0, 1.0);

//    int weight = j + i + 2;

//    // sciezka cienia
//    const Node & eyeNode = eyePath[i];
//    const Node & lightNode = lightPath[lightPath.size() - 1];
//    bool isShadowVisible = IsVisible(scene, eyeNode.intersectionResult.LPOINT, lightNode.intersectionResult.LPOINT);
//    if(!isShadowVisible)
//    {
//        return LightIntensity();
//    }

//    for (int lightNodeNumber = 0; lightNodeNumber <= j; ++lightNodeNumber)
//    {
//        const Node & lightNode = lightPath[lightNodeNumber];

//        if (lightNodeNumber == 0)
//        {
//            L = light->GetLightIntensity(cameraPosition, &lightNode.intersectionResult, scene->geometry);
//        }
//        else
//        {
//            L = light->GetLightIntensity(cameraPosition, &lightNode.intersectionResult, scene->geometry, lightPath[lightNodeNumber - 1].intersectionResult.LPOINT, L * lightPath[lightNodeNumber - 1].weight / weight);
//        }

//        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::EvalPath - lightNode - L: " << L;
//    }


//    for (int eyeNodeNumber = i; eyeNodeNumber >= 0; --eyeNodeNumber)
//    {
//        const Node & eyeNode = eyePath[eyeNodeNumber];

//        if (eyeNodeNumber == i)
//        {
//            L = light->GetLightIntensity(cameraPosition, &eyeNode.intersectionResult, scene->geometry, lightPath[j].intersectionResult.LPOINT, L * lightPath[j].weight / weight);
//        }
//        else
//        {
//            L = light->GetLightIntensity(cameraPosition, &eyeNode.intersectionResult, scene->geometry, eyePath[i + 1].intersectionResult.LPOINT, L * eyePath[i + 1].weight / weight);
//        }

//        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::EvalPath - eyeNode - L: " << L;
//    }

//    if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::EvalPath - L: " << L;
//    return L;
//}

float BidirectionalPathTracer::WeightPath(int i, int j) const
{
    return (float) (i + j);
}

bool BidirectionalPathTracer::FindIntersectionInScene(const Scene* const &scene, const Ray &ray, IntersectionResult &intersection) const
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
        // qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - NaN";
        // qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - intersection: " << intersection;
        // qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - objectId: " << objectId;
        // qDebug() << __LINE__ << ". BidirectionalPathTracer::FindIntersectionInScene - object: " << scene->geometry;
        throw 1l;
        return false;
    }

    return objectId != -1;
}

bool BidirectionalPathTracer::IsVisible(const Scene* const &scene, const Vector3 &a, const Vector3 &b) const
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

Ray BidirectionalPathTracer::RussianRoulette(const IntersectionResult &intersection,
                                             std::vector<Node> &path,
                                             const Vector3 &normal,
                                             const Vector3 &rayInDirection) const
{
    const Vector3 &origin = intersection.LPOINT;
    const Material* const &material = intersection.object->GetMaterial();

    if(material->type == REFLECTIVE)
    {
        if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - 1";
        Vector3 reflected = rayInDirection.Reflect(normal);
        reflected.Normalize();
        Ray rayOut(origin + reflected * BIAS, reflected);
        path.push_back(Node(intersection, 1.0f, 1.0f / path.size(), rayInDirection, rayOut.direction));
        return rayOut;
    }
    else if(material->type == REFRACTIVE)
    {
        double reflectionCoef = std::max(0.0f, std::min(1.0f, 0.05f + 0.11f * (1.0f + rayInDirection.DotProduct(normal))));

        const RefractiveMaterial* const &mat = (RefractiveMaterial*)material;

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

        if (qrand() % 2 == 0)
        {
            if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - 2";
            Ray rayOut(origin + refracted * BIAS, refracted);
            path.push_back(Node(intersection, 1.0f - reflectionCoef, 1.0f / path.size(), rayInDirection, rayOut.direction));
            return rayOut;
        }
        else
        {
            if(DEBUG) qDebug() << __LINE__ << ". BidirectionalPathTracer::RussianRoulette - 3";
            Vector3 reflected = rayInDirection.Reflect(normal);
            reflected.Normalize();
            Ray rayOut(origin + reflected * BIAS, reflected);
            path.push_back(Node(intersection, reflectionCoef, 1.0f / path.size(), rayInDirection, rayOut.direction));
            return rayOut;
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
        path.push_back(Node(intersection, weight, 1.0f / path.size(), rayInDirection, rayOut.direction));

        return rayOut;
    }

    throw NULL;
}

bool BidirectionalPathTracer::shouldReflect(const Material* const &material) const
{
    bool should = false;
    switch (material->type) {
    case DIFFUSE:
    {
        DiffuseMaterial* diffuse = (DiffuseMaterial*)(material);
        break;
    }
    case REFLECTIVE:
    {
        ReflectiveMaterial* reflective = (ReflectiveMaterial*)(material);
        break;
    }
    case REFRACTIVE:
    {
        RefractiveMaterial* refractive = (RefractiveMaterial*)(material);
        break;
    }
    default:
    {
        break;
    }
    }
    return should;
}

std::vector<Node>& BidirectionalPathTracer::GeneratePath(std::vector<Node> &path, const Scene* const &scene, const Ray &rayIn, const int &maxReflections) const
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
            reflections++;

            Ray rayOut = RussianRoulette(intersection, path, normal, rayInDirection);
            intersectionInScene = FindIntersectionInScene(scene, rayOut, intersection);
        }
        while (reflections <= maxReflections && intersectionInScene);
    }

    return path;
}

#undef DEBUG
#undef DEBUGRR
