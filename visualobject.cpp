#include "visualobject.h"
#include "shader.h"

VisualObject::VisualObject()
{
    mTransMatrix.setToIdentity();
    mRotMatrix.setToIdentity();
    mScaleMatrix.setToIdentity();
}

VisualObject::~VisualObject()
{
//    glDeleteVertexArrays(1, &mVAO); /TODO, fix this
//    glDeleteBuffers(1, &mVBO);
}
void VisualObject::move(gsl::Vector3D vec)
{
    mTransMatrix.setToIdentity();
    mTransMatrix.translate(vec);
}
void VisualObject::scale(gsl::Vector3D vec)
{
    mScaleMatrix.setToIdentity();
    mScaleMatrix.scale(vec);
}
void VisualObject::rotate(gsl::Vector3D vec)
{
    mRotMatrix.rotateX(vec.x);
    mRotMatrix.rotateY(vec.y);
    mRotMatrix.rotateZ(vec.z);
}
gsl::Vector3D VisualObject::getPosition()
{
    return mTransMatrix.getPosition();
}
// I prefer having three separate matrices instead of the one model matrix.
gsl::Matrix4x4 VisualObject::getModelMatrix()
{
    // Create the model matrix
    mMatrix = mTransMatrix * mRotMatrix * mScaleMatrix;
    return mMatrix;
}

bool VisualObject::getUseTextures() const
{
    return useTextures;
}

void VisualObject::setUseTextures(bool value)
{
    useTextures = value;
}
void VisualObject::init()
{
}
