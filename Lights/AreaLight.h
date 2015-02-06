#ifndef AREALIGHT_H
#define AREALIGHT_H
#include "AmbientLight.h"
#include "Math/Vector2.h"
#include <QVector>
#include "POINTLight.h"

class AreaLight : public AmbientLight
{
public:
    /**
      Creates array of LPOINT lights, with given center and size.
      All lights in array have the same color and attenuation
      */
    AreaLight(Vector3 center, Vector2 size, Color color, Vector3 attenuation, int density);
    ~AreaLight();

    LightIntensity GetLightIntensity(const Vector3 &cameraPosition,
                                     const IntersectionResult* const &ir,
                                     const QList<Geometry *> &geometry) const;

    LightIntensity GetLightIntensity(const Vector3 &cameraPosition,
                                             const IntersectionResult* const &ir,
                                             const QList<Geometry *> &geometry,
                                             const Vector3 &position,
                                             const LightIntensity &color) const;

    Ray GetPhoton(bool useProjectionMap=false) const;
    void CreateProjectionMap(const Scene* scene);
    float GetProjectionMapRatio() const;

protected:
    bool IsInShadow(const IntersectionResult* const &ir, const QList<Geometry *> &geometry) const;
private:
    QVector<POINTLight*> lights;
    QVector<bool> inShadows;
    Vector3 center;
    Vector2 size;
};

#endif // AREALIGHT_H
