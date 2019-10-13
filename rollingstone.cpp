#include "rollingstone.h"
#include "collision.h"
#include "trianglesurface.h"

RollingStone::RollingStone() {
    collisionSystem = new Collision;
    J = mMass * (radius() * radius() + radius() * radius()) / 12000;
}
void RollingStone::updateVelocity(const vec3 &baryc) {
    //    if (gsl::within(baryc.x) && gsl::within(baryc.y) && (baryc.x + baryc.y) <= 1) {
    if (!currentTriangle.empty()) {
        auto [normal, position] = collisionSystem->getBallNormal(baryc, currentTriangle);
        calculateVelocity(normal, position);
    }
    /*    }*/ /*else
        mVelocity = gravity * mSpeed;*/
}
void RollingStone::update() {
}
vec3 RollingStone::velocity() const {
    return mVelocity;
}
void RollingStone::calculateVelocity(vec3 normal, vec3 vecToTri) {
    //    qDebug() << "Actual velocity: "<<velocity().x << velocity().y << velocity().z;

    float distanceToTriangle = vec3::dot(getPosition() - vecToTri, normal);
    qDebug() << distanceToTriangle;
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
    //    qDebug() << velocity();
    move(newPos);

    vec3 force = gravity * mMass;
    vec3 springForce = vec3(1, 10, 1) - (vec3(radius(), radius(), radius()) * -1 * 0.5);
    //    qDebug() << springForce;
    vec3 r = getPosition() - radius();
    vec3 rxf = vec3::cross(r, springForce);
    vec3 torque = rxf * -1;
    //    force += springForce;
    if (distanceToTriangle < radius()) {
        float Ax = -gravity.y * cos(gsl::deg2rad(10));
        float Ay = gravity.y * sin(gsl::deg2rad(10));
        float Az = -gravity.y * normal.z;
        force.x = Ax;
        force.y = Ay;
        force.z = Az;
        //        float distance = radius() - distanceToTriangle;
        //        if (distance > radius())
        //            mTransMatrix.translate(normal * distance);
    }
    //    qDebug() << force;

    vec3 newAcceleration = force / mMass;
    vec3 deltaV = (mAcceleration + newAcceleration) * (0.5 * deltaTime);
    deltaV = deltaV * 10;
    mVelocity = deltaV;

    //    torque += omega * angularB;
    //    alpha = torque / J;
    //    omega += alpha * deltaTime;
    //    vec3 deltaTheta = omega * deltaTime;
    //    theta += deltaTheta;
    //    rotate(deltaTheta);
}
