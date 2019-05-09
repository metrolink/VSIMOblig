#include "beziercurve.h"
#include "Objects/gameobject.h"
#include "Shaders/shader.h"
#include "jmath.h"
#include <cmath>
BezierCurve::BezierCurve(std::vector<vec3> controlPoints, int degree) : b{controlPoints}
{
    // uncomment below to show the control points as red lines in scene
    //    for (vec3 point : controlPoints) // Place each control point in the scene
    //    {
    //        mVertices.push_back({point.getX(), point.getY(), point.getZ(), 1.f, 0.f, 0.f});
    //        control.push_back(getModelMatrix() * point);
    //    }
    for (double t{1.f}, step{0.05}; t > 0.f; t -= step)
    {
        vec3 point = Bezier(t, degree);
        mVertices.push_back({point.getX(), point.getY(), point.getZ(), 0.f, 1.f, 0.f});
        points.push_back(point); // points in world space
    }
    mVertices.push_back({b[0].getX(), b[0].getY(), b[0].getZ(), 0.f, 1.f, 0.f});
}
void BezierCurve::move(const vec3 &loc)
{
    mTransMatrix.setToIdentity();
    mTransMatrix.translate(loc);
}
void BezierCurve::setColor(float r, float g, float b)
{
    mShaderProgram->use();
    mShaderProgram->setVec3("objectColor", vec3(r, g, b));
}
void BezierCurve::init(Shader *shader)
{
    VisualObject::init(shader);

    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void BezierCurve::draw()
{
    mShaderProgram->use();
    glBindVertexArray(mVAO);
    glUniformMatrix4fv(mShaderProgram->mMatrixUniform, 1, GL_FALSE, getModelMatrix().constData());
    glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
}
/**
 * @brief BezierCurve::Bezier
 * @param t
 * @return Bezier Curve with "degree" number of control points
 */
vec3 BezierCurve::Bezier(float t, int degree)
{
    std::vector<vec3> c{b};
    t = JMath::clamp(t, 0, 1); // clamp t to 0-1 since we don't care about extended bezier curves.
    for (int k{1}; k <= degree; k++)
    {
        for (int i{0}; i < degree - k + 1; i++)
            c[i] = c[i] * (1 - t) + c[i + 1] * t;
    }
    return c[0];
}

std::vector<vec3> BezierCurve::getPoints()
{
    std::vector<vec3> worldPoints;
    for (vec3 point : points)
    {
        vec4 point4d = vec4(point, 1) * getModelMatrix();
        worldPoints.push_back(point4d);
    }
    return worldPoints;
}

std::vector<vec3> BezierCurve::getControl() const
{
    return control;
}
