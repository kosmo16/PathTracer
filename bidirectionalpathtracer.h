#ifndef BIDIRECTIONALPATHTRACER_H
#define BIDIRECTIONALPATHTRACER_H
#include "LightIntensity.h"
#include "Math/Ray.h"

class Scene;

class BiDirectionalPathTracer
{
public:
    BiDirectionalPathTracer();

    /**
      Traces ray with use of photon map.
      \param ray ray to trace
      \param scene scene
      \param cameraPosition position of camera
      \param maxReflections maximum number of ray reflections
      \param exposure exposure of colors
      */
    LightIntensity TracePath(const Ray&ray, Scene*scene, const Vector3 cameraPosition,
                            int maxReflections);
};

#endif // BIDIRECTIONALPATHTRACER_H
