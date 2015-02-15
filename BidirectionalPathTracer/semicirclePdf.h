#ifndef __SEMICIRCLE_PDF_H__
#define __SEMICIRCLE_PDF_H__

#include "BidirectionalPathTracer/pdf.h"

#include <iostream>

using namespace std;

class SemicirclePdf : public Pdf
{
public:
    SemicirclePdf();
    virtual ~SemicirclePdf();

    virtual Vector3 computeDirection(const Vector3 &incomingDirection, const Vector3 &normal) const;
};

#endif
