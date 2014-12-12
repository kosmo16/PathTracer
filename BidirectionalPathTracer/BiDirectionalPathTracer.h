#ifndef BIDIRECTIONALPATHTRACER_H
#define BIDIRECTIONALPATHTRACER_H

#include "Lights/POINTLight.h"
#include "Math/Ray.h"
#include "Node.h"

class Scene;

class BidirectionalPathTracer
{

private:
    static const int EYE_REFLECTIONS = 2;
    static const int LIGHT_REFLECTIONS = 2;

public:
    BidirectionalPathTracer();

    /**
      Calculate Light Intenisty using Bidirectional Path Tracing
      \param ray ray to trace
      \param scene scene
      */
    LightIntensity CalculateLightIntensity(Scene *scene, const Ray &ray);
    void GeneratePath(std::vector<Node> &path, Scene *scene, const Ray &rayIn, const int &maxReflections);
    bool FindIntersectionInScene(Scene *scene, const Ray &ray, const IntersectionResult &intersection);
    float WeightPath(int i, int j);
    float EvalPath(Scene *scene, std::vector eyePath, int i, std::vector lightPath, int j);
};

#endif // BIDIRECTIONALPATHTRACER_H
