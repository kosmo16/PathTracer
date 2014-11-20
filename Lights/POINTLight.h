#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "AmbientLight.h"

class Scene;

class POINTLight : public AmbientLight
{
public:
    POINTLight();
    /**
      Constructs LPOINT light with given position, color and attenuation parameters
      */
    POINTLight(Vector3 position, Color color, Vector3 attenuation);
    ~POINTLight();

    LightIntensity GetLightIntensity(Vector3 cameraPosition, IntersectionResult *ir,QList<Geometry *> &geometry );

    /**
      Gets position of light
      */
    Vector3 GetPosition() const;
    Ray GetPhoton(bool useProjectionMap=false) const;

    void CreateProjectionMap(const Scene* scene);

    float GetProjectionMapRatio() const;

protected:
    bool IsInShadow(IntersectionResult *ir, QList<Geometry *> &geometry);

private:
    Vector3 position;
    Vector3 attenuation;

    Texture* projectionMap;
};

#endif // POINTLIGHT_H
