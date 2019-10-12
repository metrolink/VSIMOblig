#include "lasmap.h"
#include "math_constants.h"
#include "vertex.h"
#include <QDebug>
#include <cmath>

LasMap::LasMap()
{
    Vertex v{};
    v.set_xyz(0, 0, 0);
    v.set_rgb(0, 1, 0);
    v.set_uv(0, 0);
    mVertices.push_back(v);
    v.set_xyz(2, 0, 0);
    v.set_rgb(0, 1, 0);
    v.set_uv(1, 0);
    mVertices.push_back(v);
    v.set_xyz(2, 0, 2);
    v.set_rgb(0, 1, 0);
    v.set_uv(1, 1);
    mVertices.push_back(v);
    v.set_xyz(0, 0, 0);
    v.set_rgb(0, 1, 0);
    v.set_uv(0, 0);
    mVertices.push_back(v);
    v.set_xyz(2, 0, 2);
    v.set_rgb(0, 1, 0);
    v.set_uv(1, 1);
    mVertices.push_back(v);
    v.set_xyz(0, 0, 2);
    v.set_rgb(0, 1, 0);
    v.set_uv(0, 1);
    mVertices.push_back(v);
}

LasMap::~LasMap()
{

}

void LasMap::init()
{
    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glPointSize(6.f);

    glBindVertexArray(0);
}

void LasMap::draw()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mVertices.size());
}

void LasMap::readFile(std::string filename)
{

}
