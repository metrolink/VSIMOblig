#include "plane.h"

PLANE::PLANE(const vec3 &origin, const vec3 &normal) {
    this->normal = normal;
    this->origin = origin;
    equation[0] = normal.x;
    equation[1] = normal.y;
    equation[2] = normal.z;
    equation[3] = -(normal.x * origin.x +
                    normal.y * origin.y +
                    normal.z * origin.z);
}
// Construct from triangle
PLANE::PLANE(const vec3 &p1, const vec3 &p2, const vec3 &p3) {
    normal = vec3::cross((p2 - p1), (p3 - p1));
    normal.normalize();
    origin = p1;
    equation[0] = normal.x;
    equation[1] = normal.y;
    equation[2] = normal.z;
    equation[3] = -(normal.x * origin.x +
                    normal.y * origin.y +
                    normal.z * origin.z);
}

bool PLANE::isFrontFacingTo(const vec3 &direction) const {
    double dot = vec3::dot(normal, direction);
    return (dot <= 0);
}

double PLANE::signedDistanceTo(const vec3 &point) const {
    // Dot product of point and normal + equation[3]
    return vec3::dot(point, normal) + equation[3];
}
