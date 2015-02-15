#ifndef __HEMISPHERE_PDF_H__
#define __HEMISPHERE_PDF_H__

#include "BidirectionalPathTracer/pdf.h"

class HemispherePdf : public Pdf
{
public:
    HemispherePdf();
    virtual ~HemispherePdf();

    virtual Vector3 computeDirection(const Vector3 &incomingDirection, const Vector3 &normal) const;
};

#endif
