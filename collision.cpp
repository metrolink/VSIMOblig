#include "collision.h"
#include "trianglesurface.h"
Collision::Collision() {
}
/**
 * @brief Collision::barycentricCoordinates computes the barycentric coordinates u,v,w for a point with respect to a triangle
 * @param point The point we compute the coordinates for
 * @param pointA Point A of the triangle
 * @param pointB Point B of the triangle
 * @param pointC Point C of the triangle
 * @return
 */
vec3 Collision::barycentricCoordinates(const vec3 &point, const vec3 &pointA, const vec3 &pointB, const vec3 &pointC) {
    vec3 v0 = pointB - pointA;
    vec3 v1 = pointC - pointA;
    vec3 v2 = point - pointA;

    float v0Sqr = vec3::dot(v0, v0);
    float v0Dotv1 = vec3::dot(v0, v1);
    float v1Sqr = vec3::dot(v1, v1);
    float v2Dotv0 = vec3::dot(v2, v0);
    float v2Dotv1 = vec3::dot(v2, v1);
    float denom = v0Sqr * v1Sqr - v0Dotv1 * v0Dotv1;
    vec3 baryc;
    baryc.x = (v1Sqr * v2Dotv0 - v0Dotv1 * v2Dotv1) / denom;
    baryc.y = (v0Sqr * v2Dotv1 - v0Dotv1 * v2Dotv0) / denom;
    baryc.setZ(1.0f - baryc.x - baryc.x);
    //    qDebug() << baryc.x << baryc.y << baryc.z;
    return baryc;
}
std::tuple<vec3, vec3> Collision::getBallNormal(const vec3 &baryc, std::vector<vec3> triangle) {
    vec3 normal{0};
    vec3 tempPos{0};
    normal = vec3::cross(triangle[2] - triangle[0], triangle[1] - triangle[0]);
    normal.normalize();
    tempPos = (triangle[0] * baryc.x + triangle[1] * baryc.y + triangle[2] * baryc.z);
    qDebug() << tempPos;

    return std::tuple<vec3, vec3>(normal, tempPos);
}
