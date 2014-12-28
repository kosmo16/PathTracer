#ifndef __NORMALPDF_H__
#define __NORMALPDF_H__

#include "pdf.h"

#include "Math/Matrix4x4.h"

#include <iostream>

using namespace std;

class NormalPdf : public Pdf
{
public:
    NormalPdf();
    virtual ~NormalPdf();

    virtual Vector3 computeDirection(const Vector3 & incomingDirection, const Vector3 & normal) const;
};

#endif
