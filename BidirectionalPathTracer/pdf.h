#ifndef __PDF_H__
#define __PDF_H__

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

class Pdf
{
protected:
    /**
     * @brief Rotates vector v around axis by the angle.
     * @param v vector to rotate
     * @param axis rotation axis
     * @param angle angle of rotation
     * @return rotated angle
     */
    static Vector3 rotate(const Vector3 &v,
                          const Vector3 &axis,
                          float angle);

    /**
     * @brief Rotates given vector by random angle
     * from interval [-PI/2, PI/2] around given axis.
     * @param vectorToRotate given vector to rotate
     * @param axis rotation axis
     * @return
     */
    static const Vector3& randomRotate(const Vector3 &vectorToRotate,
                                       const Vector3 &axis);

    /**
     * @brief Rotates given vector by random angle
     * from interval [-PI/2, PI/2] after plane described by two vectors.
     * @param vectorToRotate given vector to rotate
     * @param planeVector1 vector lying on the plane
     * @param planeVector2 vector lying on the plane
     * @return
     */
    static const Vector3& randomRotate(const Vector3 &vectorToRotate,
                                       const Vector3 &planeVector1,
                                       const Vector3 &planeVector2);

public:
    Pdf();
    virtual ~Pdf();

    virtual Vector3 computeDirection(const Vector3 & incomingDirection, const Vector3 & normal) const = 0;
};

#endif
