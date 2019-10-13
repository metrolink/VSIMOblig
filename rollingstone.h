#ifndef ROLLINGSTONE_H
#define ROLLINGSTONE_H

#include "octahedronball.h"
class Collision;
class TriangleSurface;
class RollingStone : public OctahedronBall {
public:
    RollingStone();
    vec3 velocity() const;
    void updateVelocity(const vec3 &baryc);

    void update();
    std::vector<vec3> currentTriangle;

private:
    vec3 alpha{0};
    vec3 omega{0};
    vec3 theta{0};
    float angularB{-7};
    float J;
    vec3 mVelocity{0};
    vec3 mAcceleration{0};
    vec3 lastAcceleration;
    float mMass{1};
    double deltaTime{0.016f};
    vec3 lastNormal;
    vec3 gravity{0, -9.81, 0};
    Collision *collisionSystem;
    void calculateVelocity(vec3 normal, vec3 distance);
};

#endif // ROLLINGSTONE_H
