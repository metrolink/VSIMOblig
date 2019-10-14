#include "rollingstone.h"
#include "collision.h"
#include "trianglesurface.h"

RollingStone::RollingStone() {
    collisionSystem = new Collision;
}

void RollingStone::update() {
    if (!currentTriangle.empty()) {
        auto [normal, distance] = collisionSystem->getBallNormal(currentTriangle, *this);
        calculateVelocity(normal, distance);
    } else
        calculateVelocity(vec3(0), radius());
}
vec3 RollingStone::velocity() const {
    return mVelocity;
}
void RollingStone::calculateVelocity(vec3 normal, double distanceToTriangle) {
    //    if (distanceToTriangle > radius()) { // Not in contact with triangle, no directional force applied
    //        normal = vec3(0);
    //        qDebug() << "X-Acceleration = " << 0 << "Y-Acceleration = " << gravity.y;
    //    } else {
    // acos(normal.y) = 30 degrees in radians
    // sin(30 degrees/0.5 rads) * gravity.y

    //        qDebug() << force;
    //        float distance = radius() - distanceToTriangle;
    //        if (distance > 0.2f)
    //            mTransMatrix.translate(normal * distance);
    //    }
    //    if (normal != lastNormal) {
    //        if (normal == vec3(0)) {
    //        } else if (lastNormal == vec3(0)) {
    //            mVelocity = (gravity + N).normalized() * vec3::dot(velocity(), (gravity + N).normalized());
    //        } else {
    //            vec3 tempNormal = normal + lastNormal;
    //            tempNormal.normalize();
    //            vec3 tempVelocity = tempNormal * vec3::dot(velocity(), tempNormal);
    //            tempVelocity = velocity() - tempVelocity * 2;
    //            mVelocity = tempVelocity;
    //        }
    //    }
    //    lastNormal = normal;

    vec3 newPos = getPosition();
    newPos += (velocity() * deltaTime) + (mAcceleration * 0.5 * pow(deltaTime, 2));
    move(newPos);

    vec3 force = gravity * mMass;

    if (distanceToTriangle <= radius()) {
        float Ax = -gravity.y * normal.y;
        float Ay = gravity.y * acos(normal.y);
        float Az = -gravity.y * normal.y;
        force.x = Ax;
        force.y = Ay;
        force.z = Az;
    }
    //    qDebug() << force;

    vec3 newAcceleration = force / mMass;
    vec3 deltaV = (mAcceleration + newAcceleration) * (0.5 * deltaTime);
    deltaV = deltaV * 100;
    mVelocity = deltaV;
}
