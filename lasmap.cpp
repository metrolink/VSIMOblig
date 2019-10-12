#include "lasmap.h"
#include "math_constants.h"
#include "vertex.h"
#include <QDebug>
#include <cmath>
#include <algorithm>

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

    //printSomePoints();
    addAllPointsToVertices();

    centerMap();
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

    glPointSize(2.f);

    glBindVertexArray(0);
}

void LasMap::draw()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mVertices.size());
}

void LasMap::printSomePoints()
{
    for (auto point = lasloader.begin() + 10; point != lasloader.end() - 25; ++point)
    {
        std::cout << "Point: (" << point->xNorm() << ", " << point->yNorm() << ", " << point->zNorm() << ")" << std::endl;
    }
}

void LasMap::addAllPointsToVertices()
{
    mVertices.clear();
    for (auto point : lasloader)
    {
            Vertex v{};
//            v.set_xyz(point.xNorm(), point.yNorm(), point.zNorm());
            v.set_xyz(point.x, point.y, point.z);
            v.set_rgb(0, 1, 0);
            v.set_uv(0, 0);
            mVertices.push_back(v);
    }

}

void LasMap::centerMap()
{
    std::vector<double> xValues;
    std::vector<double> yValues;
    std::vector<double> zValues;


    for (auto point : lasloader)
    {
        xValues.push_back(point.x);
        yValues.push_back(point.y);
        zValues.push_back(point.z);

//        xValues.push_back(point.xNorm());
//        yValues.push_back(point.yNorm());
//        zValues.push_back(point.zNorm());
    }
    std::sort(xValues.begin(), xValues.end());
    std::sort(yValues.begin(), yValues.end());
    std::sort(zValues.begin(), zValues.end());

    std::cout << xValues[0] << " ";
    std::cout << yValues[0] << " ";
    std::cout << zValues[0] << " ";

    std::cout << xValues[xValues.size() - 1] << " ";
    std::cout << yValues[yValues.size() - 1] << " ";
    std::cout << zValues[zValues.size() - 1] << " ";

//    double xTranslate = (-xValues[0] + xValues[xValues.size() - 1]) ;
//    double yTranslate = (-yValues[0] + yValues[xValues.size() - 1]) ;
//    double zTranslate = (-zValues[0] + zValues[xValues.size() - 1]) ;

    double xTranslate = ((xValues[xValues.size() - 1]) - ((xValues[xValues.size() - 1] - xValues[0]) * 0.5));
    double yTranslate = ((yValues[xValues.size() - 1]) - ((xValues[xValues.size() - 1] - xValues[0]) * 0.5));
    double zTranslate = ((zValues[xValues.size() - 1]) - ((xValues[xValues.size() - 1] - xValues[0]) * 0.5));

    int scaleNumber = 1;
    scale(scaleNumber);
    move(gsl::Vector3D(-xTranslate * scaleNumber, -yTranslate * scaleNumber, -zTranslate * scaleNumber));
    //mMatrix.translate(-xTranslate, -yTranslate, -zTranslate);
    //mMatrix.translate(2, 2, 2);
}


