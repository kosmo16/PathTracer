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
    static const int EYE_REFLECTIONS = 7;
    static const int LIGHT_REFLECTIONS = 7;

    const Brdf * const &brdf;
    const Pdf * const &pdf;

    LightIntensity GetIntensity(const Node & node) const;

    /**
     * @brief Generates ray from random light from scene.
     * @param scene scene
     * @param randomLightRay generated ray
     * @return the random light
     */
    AmbientLight* GetRandomLightRay(const Scene* const &scene, Ray &randomLightRay) const;

    std::vector<Node>& GeneratePath(std::vector<Node> &path, const Scene* const &scene, const Ray &rayIn, int maxReflections) const;

    bool FindIntersectionInScene(const Scene* const &scene, const Ray &ray, IntersectionResult &intersection) const;

    /**
     * @brief Computes weight of path determined by shadow path.
     * @param i the beginning of shadow path
     * @param j the end of shadow path
     * @return weight of path
     */
    float WeightPath(int i, int j) const;

    LightIntensity EvalPath(const Scene* const &scene,
                            const std::vector<Node> &eyePath, int i,
                            const std::vector<Node> &lightPath, int j,
                            AmbientLight *light, const Vector3 &cameraPosition) const;

    /**
     * @brief Checks if vectors a and b see each other in the scene.
     * @param scene scene
     * @param a first vector
     * @param b second vector
     * @return true if vectors a and b see each other
     */
    bool IsVisible(const Scene* const &scene, const Vector3 &a, const Vector3 &b) const;

    Ray * RussianRoulette(const IntersectionResult &intersection,
                          std::vector<Node> &path,
                          const Vector3 &normal,
                          const Vector3 &rayInDirection) const;

public:
    BidirectionalPathTracer(const Brdf* const &brdf, const Pdf* const &pdf);

    /**
      Traces ray with use of photon map.
      Calculate Light Intenisty using Bidirectional Path Tracing.
      \param ray ray to trace
      \param scene scene
      \param cameraPosition position of camera
      \param maxReflections maximum number of ray reflections
      */
    LightIntensity TracePath(const Ray &ray, const Scene* const &scene, const Vector3 &cameraPosition) const;

};

#endif
