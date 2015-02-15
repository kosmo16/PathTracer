#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "Lights/POINTLight.h"
#include "Math/Ray.h"
#include "Math/randomUtils.h"
#include "LightIntensity.h"
#include "Scene.h"
#include "Node.h"

#include <cfloat>

class PathTracer
{
public:
    PathTracer();
    ~PathTracer();

    /**
      Traces ray with use of streamed photon map.
      \param ray ray to trace
      \param scene scene
      \param cameraPosition position of camera
      \param maxReflections maximum number of ray reflections
      \param exposure exposure of colors
      \param globalMap global streamed photon map
      \param causticMap caustic streamed photon map
      */
    LightIntensity TracePath(const Ray&ray, Scene *scene, const Vector3 cameraPosition,
                            int n);

    std::vector<Node>& GeneratePath(std::vector<Node> &path, Scene*scene, const Ray &rayIn, int &maxReflections);

    bool FindIntersectionInScene(Scene* scene, const Ray &ray, IntersectionResult &intersection);
    Ray CalculateNode(const IntersectionResult &closestIntersection, std::vector<Node> &path, const Vector3 &normal, const Vector3 &rayInDirection);
};

#endif // PATHTRACER_H
