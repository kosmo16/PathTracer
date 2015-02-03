#include "AmbientLight.h"

float floatRand()
{
    static float RAND_MAX_F = static_cast<float>(RAND_MAX);
    static float ONE_RAND_MAX_F = 1.0f / RAND_MAX_F;
    return static_cast <float>(qrand()) * ONE_RAND_MAX_F;
}

AmbientLight::AmbientLight() {
    type = AMBIENT;
    color = Color(1,1,1);
}

AmbientLight::AmbientLight(Color c) : color(c) {
    type = AMBIENT;
}

bool AmbientLight::IsInShadow(const IntersectionResult* const &ir, const QList<Geometry *> &geometry) const {
    return false;
}

LightIntensity AmbientLight::GetLightIntensity(const Vector3 &cameraPosition,
                                               const IntersectionResult* const &ir,
                                               const QList<Geometry *> &geometry) const {
    Q_UNUSED(cameraPosition);
    Q_UNUSED(ir);
    Q_UNUSED(geometry);
    return color;
}

Ray AmbientLight::GetPhoton(bool useProjectionMap) const {
    return Ray();
}

void AmbientLight::CreateProjectionMap(const Scene *s) {

}

float AmbientLight::GetProjectionMapRatio() const {
    return 0;
}
