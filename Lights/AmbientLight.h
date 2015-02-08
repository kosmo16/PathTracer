#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

#include "LightIntensity.h"
#include "Math/Vector3.h"
#include "Math/Ray.h"
#include "Geometry/Geometry.h"

typedef LightIntensity Color;
class Scene;

enum LightSourceType {
    LPOINT=0,
    DIRECTIONAL,
    AREA,
    AMBIENT
};

/**
  Ambient Light, base class of all lights
  */
class AmbientLight
{
public:
    AmbientLight();
    /**
      Constructs ambient light with given color
      */
    AmbientLight(Color c);
    virtual ~AmbientLight(){}

    LightSourceType type;
    Color color;

    /**
      Get light intensity for given intersection LPOINT and camera
     */
    virtual LightIntensity GetLightIntensity(const Vector3 &cameraPosition,
                                             const IntersectionResult* const &ir,
                                             const QList<Geometry *> &geometry) const;

    /**
      Get light intensity for given intersection LPOINT and camera
     */
    virtual LightIntensity GetLightIntensity(const Vector3 &cameraPosition,
                                             const IntersectionResult* const &ir,
                                             const QList<Geometry *> &geometry,
                                             const Vector3 &position,
                                             const LightIntensity &color) const;


    /**
      Generates photon from light (only from LPOINT and area lights)
      */
    virtual Ray GetPhoton(bool useProjectionMap=false) const;
    /**
      Generates projection map for light(only from LPOINT and area lights)
      */
    virtual void CreateProjectionMap(const Scene* s);
    virtual float GetProjectionMapRatio() const;

protected:
    //checks if intersection LPOINT is in shadow by this light.
    virtual bool IsInShadow(const IntersectionResult* const &ir, const QList<Geometry *> &geometry) const;
};

#endif // AMBIENTLIGHT_H
