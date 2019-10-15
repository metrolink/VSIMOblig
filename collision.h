#ifndef COLLISION_H
#define COLLISION_H

#include "gsl_math.h"
typedef gsl::Vector3D vec3;
class LasMap;
class TriangleSurface;
class Collision {
public:
    Collision();
    vec3 barycentricCoordinates(const vec3 &point, const vec3 &pointA, const vec3 &pointB, const vec3 &pointC);
    std::tuple<bool, vec3, vec3> getBallNormal(const vec3 &ballPos, LasMap *surface);
};

#endif // COLLISION_H
