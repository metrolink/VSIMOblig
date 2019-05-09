#include "octahedronball.h"
#include <cmath>
#include <iostream>
#include <sstream>

//! \param n - the recursion level (default is 0 which makes the original Octahedron)
//!
//! The number of (duplicated) vertices are calculated based on the parameter n - the recursion level.
//! Initially we have 8 triangles, each with 3 vertices.
//! A subdivision consists of
//! - compute the midpoint of each edge in a triangel
//! - add these midpoint as vertices
//! - make three new edges by connecting new vertices
//! - preserve orientation
//! - move the three new points on the unit ball's surface by normalizing. Remember the vertex vector
//! is also a normal to the ball's surface.
//! The formula for the number of vertices is
//! m_nVertices = 3 * 8 * 4^n
//! where
//! - 3 comes from the number of vertices in a triangle,
//! - 8 comes from the original number of triangles in a regular Octahedron
//! - n is the recursion level (number of repeated subdivisions)
//!
OctahedronBall::OctahedronBall(int n, float radius)
    : m_rekursjoner(n), m_indeks(0), mRadius(radius)
{
    mVertices.reserve(3 * 8 * pow(4, m_rekursjoner));
    octahedronUnitBall();
}

// Parametre inn: xyz koordinatene til et triangle v1, v2, v3 ccw
// Bevarer orienteringen av hjørnene
//!
//! \brief OctahedronBall::lagTriangel()
//! \param v1 - position on the unit ball for vertex 1
//! \param v2 - position on the unit ball for vertex 2
//! \param v3 - position on the unit ball for vertex 3
//!
//! lagTriangel() creates vertex data for a triangle's 3 vertices. This is done in the
//! final step of recursion.
//!
// added a radius modifier, super messy though, should probably do it in a different way
void OctahedronBall::createTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3)
{
    Vertex vert1{v1.getX() * mRadius, v1.getY() * mRadius, v1.getZ() * mRadius, v1.getX() * mRadius, v1.getY() * mRadius, v1.getZ() * mRadius};
    Vertex vert2{v2.getX() * mRadius, v2.getY() * mRadius, v2.getZ() * mRadius, v2.getX() * mRadius, v2.getY() * mRadius, v2.getZ() * mRadius};
    Vertex vert3{v3.getX() * mRadius, v3.getY() * mRadius, v3.getZ() * mRadius, v3.getX() * mRadius, v3.getY() * mRadius, v3.getZ() * mRadius};

    // koordinater v.x = v1.x, v.y=v1.y, v.z=v1.z
    // rgb
    // v.set_st(0.0f, 0.0f);            // kan utelates

    // v.set_st(1.0f, 0.0f);

    // v.set_st(0.5f, 1.0f);
    mVertices.push_back(vert2);
    mVertices.push_back(vert1);
    mVertices.push_back(vert3);
}

// Rekursiv subdivisjon av triangel
//!
//! \brief OctahedronBall::subDivide() recursive subdivision of a triangel
//! \param a coordinates for vertex a
//! \param b coordinates for vertex b
//! \param c coordinates for vertex c
//! \param n level of recursion
//!
//! The function tests
//! (if n>0)
//! - three new edge midpoints are computed and normalized,
//! - four new calls to subDivide() is done - one for each new triangle, preserving orientation
//! and with decreased parameter n
//! else
//! - call lagTriangel(a, b, c)
//!
void OctahedronBall::subDivide(const vec3 &a, const vec3 &b, const vec3 &c, int n)
{
    if (n > 0)
    {
        vec3 v1 = a + b;
        v1.normalize();
        vec3 v2 = a + c;
        v2.normalize();
        vec3 v3 = c + b;
        v3.normalize();
        subDivide(a, v1, v2, n - 1);
        subDivide(c, v2, v3, n - 1);
        subDivide(b, v3, v1, n - 1);
        subDivide(v3, v2, v1, n - 1);
    }
    else
    {
        createTriangle(a, b, c);
    }
}

//!
//! \brief OctahedronBall::oktaederUnitBall() creates 8 unit ball vertices and call subDivide()
//!
//! If the parameter n of the constructor OctahedronBall() is zero, the result will be the
//! original octahedron consisting of 8 triangles with duplicated vertices.
//!
void OctahedronBall::octahedronUnitBall()
{
    vec3 v0{0, 0, 1};
    vec3 v1{1, 0, 0};
    vec3 v2{0, 1, 0};
    vec3 v3{-1, 0, 0};
    vec3 v4{0, -1, 0};
    vec3 v5{0, 0, -1};

    subDivide(v0, v1, v2, m_rekursjoner);
    subDivide(v0, v2, v3, m_rekursjoner);
    subDivide(v0, v3, v4, m_rekursjoner);
    subDivide(v0, v4, v1, m_rekursjoner);
    subDivide(v5, v2, v1, m_rekursjoner);
    subDivide(v5, v3, v2, m_rekursjoner);
    subDivide(v5, v4, v3, m_rekursjoner);
    subDivide(v5, v1, v4, m_rekursjoner);
}

void OctahedronBall::init()
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

    glBindVertexArray(0);
}
void OctahedronBall::draw()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
}
