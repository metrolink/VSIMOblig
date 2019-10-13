#ifndef PLANE_H
#define PLANE_H
#include "gsl_math.h"

typedef gsl::Vector3D vec3;

class PLANE {
public:
    float equation[4];
    vec3 origin;
    vec3 normal;

    PLANE(const vec3 &origin, const vec3 &normal);
    PLANE(const vec3 &p1, const vec3 &p2, const vec3 &p3);

    bool isFrontFacingTo(const vec3 &direction) const;
    double signedDistanceTo(const vec3 &point) const;
};

#endif // PLANE_H
