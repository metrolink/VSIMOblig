#ifndef ROLLINGSTONE_H
#define ROLLINGSTONE_H

#include "octahedronball.h"
class Collision;
class TriangleSurface;
class RollingStone : public OctahedronBall {
public:
    RollingStone();
    vec3 velocity() const;

    void update();
    std::vector<vec3> currentTriangle;

private:
    vec3 mVelocity{0};
    vec3 mAcceleration{0};
    vec3 lastAcceleration;
    float mMass{1};
    double deltaTime{0.016f};
    vec3 gravity{0, -9.81, 0};
    Collision *collisionSystem;
    void calculateVelocity(vec3 normal, double distanceToTriangle);
};

#endif // ROLLINGSTONE_H
