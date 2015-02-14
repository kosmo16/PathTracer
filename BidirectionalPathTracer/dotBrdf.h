#ifndef __DOT_BRDF_H__
#define __DOT_BRDF_H__

#include "BidirectionalPathTracer/brdf.h"

class DotBrdf : public Brdf
{
public:
    DotBrdf();
    virtual ~DotBrdf();

    virtual float computeRatio(const Vector3 &incomingDirection, const Vector3 &outcomingDirection, const Vector3 &normal) const;
};

#endif
