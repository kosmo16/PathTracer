#ifndef __BIDIRECTIONAL_PATHTRACER_H__
#define __BIDIRECTIONAL_PATHTRACER_H__

#include "LightIntensity.h"
#include "Lights/POINTLight.h"
#include "Math/Ray.h"
#include "brdf.h"
#include "Node.h"
#include "pdf.h"
#include "Scene.h"

#include <cfloat>

class BidirectionalPathTracer
{

private:
    static const int EYE_REFLECTIONS = 2;
    static const int LIGHT_REFLECTIONS = 2;

    Brdf * const brdf;
    Pdf * const pdf;

public:
    BidirectionalPathTracer(Brdf * const brdf, Pdf * const pdf);

    /**
      Traces ray with use of photon map.
      \param ray ray to trace
      \param scene scene
      \param cameraPosition position of camera
      \param maxReflections maximum number of ray reflections
      \param exposure exposure of colors
      */
    LightIntensity TracePath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections);

    void GetPath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections);
    Ray GetRandomLightRay(Scene *scene);
    /**
      Calculate Light Intenisty using Bidirectional Path Tracing
      \param ray ray to trace
      \param scene scene
      */
    LightIntensity CalculateLightIntensity(Scene *scene, const Ray &ray, const Vector3 cameraPosition);
    std::vector<Node> GeneratePath(std::vector<Node> &path, Scene *scene, const Ray &rayIn, const int &maxReflections);
    bool FindIntersectionInScene(Scene *scene, const Ray &ray, IntersectionResult &intersection);
    float WeightPath(int i, int j);
    LightIntensity EvalPath(Scene *scene,
                            const std::vector<Node> &eyePath, int i,
                            const std::vector<Node> &lightPath, int j);
    bool IsVisible(Scene *scene, const Vector3 &a, const Vector3 &b);
};

#endif
