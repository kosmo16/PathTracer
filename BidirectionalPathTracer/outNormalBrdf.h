#ifndef __OUT_NORMAL_BRDF_H__
#define __OUT_NORMAL_BRDF_H__

#include "BidirectionalPathTracer/brdf.h"

class OutNormalBrdf : public Brdf
{
public:
    OutNormalBrdf();
    ~OutNormalBrdf();

    float computeRatio(const Vector3 &incomingDirection, const Vector3 &outcomingDirection, const Vector3 &normal) const;
};

#endif
