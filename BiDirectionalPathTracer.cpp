#include "BiDirectionalPathTracer.h"

BiDirectionalPathTracer::BiDirectionalPathTracer()
{
}

LightIntensity BiDirectionalPathTracer::TracePath(const Ray &ray, Scene *scene, const Vector3 cameraPosition, int maxReflections)
{
    LightIntensity resultIntensity;

    const int remainingEyeReflections = maxReflections;
    const int remainingLightReflections = maxReflections;
    int eyeReflectionsCount = 0;
    int eyeReflectionsCount = 0;


}
