#ifndef __BIDIRECTIONAL_PATHTRACER_H__
#define __BIDIRECTIONAL_PATHTRACER_H__

#include "Lights/POINTLight.h"
#include "Math/Ray.h"
#include "brdf.h"
#include "LightIntensity.h"
#include "Node.h"
#include "pdf.h"
#include "Scene.h"

#include <cfloat>

class BidirectionalPathTracer
{
private:
    static const int EYE_REFLECTIONS = 5;
    static const int LIGHT_REFLECTIONS = 5;

    const Brdf* const &brdf;
    const Pdf* const &pdf;

    LightIntensity GetIntensity(const Node & node) const;

    AmbientLight* GetRandomLightRay(const Scene* const &scene, Ray &randomLightRay) const;

    /**
      Calculate Light Intenisty using Bidirectional Path Tracing
      \param ray ray to trace
      \param scene scene
      */
    LightIntensity CalculateLightIntensity(const Scene* const &scene, const Ray &ray, const Vector3 cameraPosition) const;

    std::vector<Node>& GeneratePath(std::vector<Node> &path, const Scene* const &scene, const Ray &rayIn, const int &maxReflections) const;

    bool FindIntersectionInScene(const Scene* const &scene, const Ray &ray, IntersectionResult &intersection) const;

    float WeightPath(int i, int j) const;

    LightIntensity EvalPath(const Scene* const &scene,
                                                     const std::vector<Node> &eye, int nEye,
                                                     const std::vector<Node> &light, int nLight) const;

    LightIntensity EvalPath(const Scene* const &scene,
                            const std::vector<Node> &eyePath, int i,
                            const std::vector<Node> &lightPath, int j,
                            AmbientLight *light, const Vector3 &cameraPosition) const;

    bool IsVisible(const Scene* const &scene, const Vector3 &a, const Vector3 &b) const;

    Ray RussianRoulette(const IntersectionResult &intersection,
                        std::vector<Node> &path,
                        const Vector3 &normal,
                        const Vector3 &rayInDirection) const;

    bool shouldReflect(const Material* const &material) const;

public:
    BidirectionalPathTracer(const Brdf* const &brdf, const Pdf* const &pdf);

    /**
      Traces ray with use of photon map.
      \param ray ray to trace
      \param scene scene
      \param cameraPosition position of camera
      \param maxReflections maximum number of ray reflections
      */
    LightIntensity TracePath(const Ray &ray, const Scene* const &scene, const Vector3 &cameraPosition) const;

};

#endif
