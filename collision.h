#ifndef COLLISION_H
#define COLLISION_H

#include "gsl_math.h"
typedef gsl::Vector3D vec3;
class TriangleSurface;
class RollingStone;
class Collision {
public:
    Collision();
    std::tuple<vec3, double> getBallNormal(std::vector<vec3> triangle, RollingStone &ball);
    vec3 barycentricCoordinates(const vec3 &point, const vec3 &pointA, const vec3 &pointB, const vec3 &pointC);
};

#endif // COLLISION_H
