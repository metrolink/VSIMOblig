#include "tree.h"
#include "camera.h"
#include "octahedronball.h"
Tree::Tree()
{
    std::string cylinder = "../VSIMOblig/Assets/Cylinder.txt";
    readFile(cylinder);
    VisualObject::move(gsl::Vector3D(-1.5, 0, 3.5));
    scale(gsl::Vector3D(0.1, 0.1, 1));
    rotate(gsl::Vector3D(90, 0, 0));
    ball = new OctahedronBall();
    ball->move(gsl::Vector3D(-1.5, 2, 3.5));
}
void Tree::init(Camera *cam, Shader *shader, GLint mMatrixUniform, GLint vMatrixUniform, GLint pMatrixUniform)
{
    shaderProgram = shader;
    mCurrentCamera = cam;
    modelMatrixUniform = mMatrixUniform;
    viewMatrixUniform = vMatrixUniform;
    projMatrixUniform = pMatrixUniform;
    TriangleSurface::init();
    ball->init();
}
void Tree::move(gsl::Vector3D vec)
{
    VisualObject::move(vec);
    ball->move(vec + gsl::Vector3D(0, 1, 0));
}
void Tree::draw()
{
    TriangleSurface::draw();
    glUseProgram(shaderProgram->getProgram());
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
    glUniformMatrix4fv(projMatrixUniform, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_TRUE, ball->getModelMatrix().constData());
    ball->draw();
}
