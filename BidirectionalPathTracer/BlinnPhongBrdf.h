#ifndef BLINNPHONGBRDF_H
#define BLINNPHONGBRDF_H

#include "brdf.h"

class BlinnPhongBrdf : public Brdf
{
private:
    static const int n = 12;
    static const float R_d = 1.2345f;

    static float fDiff(float f0, const Vector3 & N, const Vector3 & L);
    static float fSpec(float f0, const Vector3 & E, const Vector3 & H);

public:
    BlinnPhongBrdf();

    virtual float computeRatio(
            const Vector3 & incomingDirection,
            const Vector3 & outcomingDirection,
            const Vector3 & normal) const;
};

#endif // BLINNPHONGBRDF_H
