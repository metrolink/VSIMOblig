#ifndef TREE_H
#define TREE_H
#include "trianglesurface.h"
class OctahedronBall;
class Camera;
class Tree : public TriangleSurface
{
public:
    Tree();
    void draw() override;

    void init(Camera *cam, Shader *shader, GLint mMatrixUniform, GLint vMatrixUniform, GLint pMatrixUniform);

    void move(gsl::Vector3D vec) override;

private:
    using TriangleSurface::init;
    Camera *mCurrentCamera;
    OctahedronBall *ball;
    Shader *shaderProgram;
    GLint modelMatrixUniform, viewMatrixUniform, projMatrixUniform;
};

#endif // TREE_H
