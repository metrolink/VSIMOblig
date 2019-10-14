#include "lasmap.h"
#include "math_constants.h"
#include "vertex.h"
#include <QDebug>
#include <cmath>
#include <algorithm>

LasMap::LasMap()
{
//    Vertex v{};
//    v.set_xyz(0, 0, 0);
//    v.set_rgb(0, 1, 0);
//    v.set_uv(0, 0);
//    mVertices.push_back(v);
//    v.set_xyz(2, 0, 0);
//    v.set_rgb(0, 1, 0);
//    v.set_uv(1, 0);
//    mVertices.push_back(v);
//    v.set_xyz(2, 0, 2);
//    v.set_rgb(0, 1, 0);
//    v.set_uv(1, 1);
//    mVertices.push_back(v);
//    v.set_xyz(0, 0, 0);
//    v.set_rgb(0, 1, 0);
//    v.set_uv(0, 0);
//    mVertices.push_back(v);
//    v.set_xyz(2, 0, 2);
//    v.set_rgb(0, 1, 0);
//    v.set_uv(1, 1);
//    mVertices.push_back(v);
//    v.set_xyz(0, 0, 2);
//    v.set_rgb(0, 1, 0);
//    v.set_uv(0, 1);
//    mVertices.push_back(v);

    //printSomePoints();
    readFile("../VSIMOblig/LASdata/fuck.txt");
//    readFile("../VSIMOblig/LASdata/fuck2.txt");
//    readFile("../VSIMOblig/LASdata/fuck3.txt");
//    readFile("../VSIMOblig/LASdata/fuck4.txt");
//    readFile("../VSIMOblig/LASdata/fuck5.txt");
//    readFile("../VSIMOblig/LASdata/33-1-497-327-20.txt");
    normalizePoints();
    addAllPointsToVertices();
    //centerMap();

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

    glPointSize(5.f);

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
    for (auto point : points)
    {
            Vertex v{};
            v.set_xyz(point.x, point.y, point.z);
            v.set_rgb(point.x/scaleFactor, point.z/scaleFactor, 0.5);
            v.set_uv(0, 0);
            mVertices.push_back(v);
    }

    //glPointSize(5);
    std::vector<gsl::Vector3D> tempPoints;
    tempPoints = mapToGrid(points, 10, 10, gsl::Vector3D(xMin, yMin, zMin), gsl::Vector3D(xMax, yMax, zMax));
    for (auto point : tempPoints)
    {
            Vertex v{};
            v.set_xyz(point.x, point.y + 0.5f, point.z);
            v.set_rgb(0, 1, 0);
            v.set_uv(0, 0);
            mVertices.push_back(v);
    }
    std::cout << tempPoints.size();
}

void LasMap::normalizePoints()
{
    std::vector<float> xValues;
    std::vector<float> zValues;
    std::vector<float> yValues;

    for (auto point : points)
    {
        xValues.push_back(point.x);
        yValues.push_back(point.y);
        zValues.push_back(point.z);
    }
    std::sort(xValues.begin(), xValues.end());
    std::sort(yValues.begin(), yValues.end());
    std::sort(zValues.begin(), zValues.end());

    xMin = xValues[0];
    yMin = yValues[0];
    zMin = zValues[0];

    xMax = xValues[xValues.size() - 1];
    yMax = yValues[yValues.size() - 1];
    zMax = zValues[zValues.size() - 1];

    for (auto &point : points)
    {
        point.x = ((point.x - xMin)/(xMax - xMin) - 0.5) * scaleFactor;
        point.y = ((point.y - yMin)/(yMax - yMin) - 0.5) * -scaleFactor;
        point.z = ((point.z - zMin)/(zMax - zMin) - 0.5) * scaleFactor;

//        point.x += 2;
//        point.y += 1;
//        point.z += 4;

//        point.setX((point.x - xMin)/(xMax - xMin));
//        point.setY((point.y - yMin)/(yMax - yMin));
//        point.setZ((point.z - zMin)/(zMax - zMin));
    }

    xValues.clear();
    yValues.clear();
    zValues.clear();
    for (auto point : points)
    {
        xValues.push_back(point.x);
        yValues.push_back(point.y);
        zValues.push_back(point.z);
    }
    std::sort(xValues.begin(), xValues.end());
    std::sort(yValues.begin(), yValues.end());
    std::sort(zValues.begin(), zValues.end());

    xMin = xValues[0];
    yMin = yValues[0];
    zMin = zValues[0];

    xMax = xValues[xValues.size() - 1];
    yMax = yValues[yValues.size() - 1];
    zMax = zValues[zValues.size() - 1];

//    for (int i = 0; i < 5; ++i)
//    {
//        std::cout << points[i].getX() << " " << points[i].getY() << " " << points[i].getZ() << "\n";
//    }

//    double xTranslate = ((xValues[xValues.size() - 1]) - ((xValues[xValues.size() - 1] - xValues[0]) * 0.5));
//    double yTranslate = ((zValues[xValues.size() - 1]) - ((zValues[xValues.size() - 1] - zValues[0]) * 0.5));
//    double zTranslate = ((yValues[xValues.size() - 1]) - ((yValues[xValues.size() - 1] - yValues[0]) * 0.5));

//    double scaleNumber = 1;
//    scale(scaleNumber);
//    move(gsl::Vector3D(-xTranslate * scaleNumber, -zTranslate * scaleNumber, -yTranslate * scaleNumber));
    //mMatrix.translate(-xTranslate, -yTranslate, -zTranslate);
    //mMatrix.translate(2, 2, 2);


    ////    move(gsl::Vector3D(-483197.75, -7569861.26, 0.70));
}

float LasMap::length(const gsl::Vector3D& a, const gsl::Vector3D& b)
{
    return static_cast<float>(std::sqrt(std::pow(a.x + b.x, 2) + std::pow(a.y + b.y, 2) + std::pow(a.z + b.z, 2)));
}

std::vector<gsl::Vector3D> LasMap::mapToGrid(const std::vector<gsl::Vector3D> &points, int xGrid, int zGrid, gsl::Vector3D min, gsl::Vector3D max)
{
    std::vector<std::pair<gsl::Vector3D, unsigned int>> grid;
    grid.resize(xGrid * zGrid);

    for (auto point : points)
    {
        int closestIndex[2]{0, 0};
        for (int z{0}; z < zGrid; ++z)
        {
            for (int x{0}; x < xGrid; ++x)
            {
                gsl::Vector3D gridPoint{
                    x * ((max.x - min.x) / xGrid) + min.x,
                    0,
                    z * ((max.z - min.z) / zGrid) + min.z
                };

                gsl::Vector3D lastClosestPoint{
                    closestIndex[0] * ((max.x - min.x) / xGrid) + min.x,
                    0,
                    closestIndex[1] * ((max.z - min.z) / zGrid) + min.z
                };

                if ((gsl::Vector3D{point.x, 0, point.z} - gridPoint).length() < (gsl::Vector3D{point.x, 0, point.z} - lastClosestPoint).length())
                {
                    closestIndex[0] = x;
                    closestIndex[1] = z;
                }
            }
        }

        // std::cout << "point is: " << point << std::endl;

        auto& p = grid.at(closestIndex[0] + closestIndex[1] * zGrid);
        p.first += point;
        ++p.second;
    }

    for (auto &p : grid)
        std::cout << "p before: " << p.first << std::endl;

    for (auto &p : grid)
        p.first = (0 < p.second) ? p.first / static_cast<float>(p.second) : gsl::Vector3D{0, 0, 0};

    for (auto &p : grid)
        std::cout << "p after: " << p.first << std::endl;

    for (int z{0}; z < zGrid; ++z)
    {
        for (int x{0}; x < xGrid; ++x)
        {
            auto& p = grid.at(x + z * zGrid);
            p.first.x = x * ((max.x - min.x) / xGrid) + min.x;
            p.first.z = z * ((max.z - min.z) / zGrid) + min.z;
        }
    }

    // convert pair into only first of pair
    std::vector<gsl::Vector3D> outputs{};
    std::transform(grid.begin(), grid.end(), std::back_inserter(outputs), [](const std::pair<gsl::Vector3D, unsigned int>& p){
        return p.first;
    });

    return outputs;
}

void LasMap::readFile()
{
//    LASreadOpener lasreadopener;
//    lasreadopener.set_file_name("../VSIMOblig/LASdata/33-1-497-327-20.las");
//    LASreader* lasreader = lasreadopener.open();

//    while (lasreader->read_point())
//    {
//        Vertex v{};
////            v.set_xyz(point.xNorm(), point.yNorm(), point.zNorm());
//        v.set_xyz(lasreader->get_x(), lasreader->get_y(), lasreader->get_z());
//        v.set_rgb(0, 1, 0);
//        v.set_uv(0, 0);
//        mVertices.push_back(v);
//    }

}

void LasMap::readFile(std::string filename)
{
    std::ifstream inn;

    inn.open(filename);

    if (inn.is_open())
    {
        unsigned int n;
        gsl::Vector3D vertex;
        inn >> n;
        points.reserve(n);
        for (unsigned int i = 0; i < n; i++)
        {
            inn >> vertex;
            points.push_back(vertex);

            std::string str;
            std::getline(inn, str);
        }
        inn.close();

        unsigned long long N = points.size() / 4;
        for (unsigned long long i = 0; i < N; ++i)
            points.at(i) = points.at(i * 4);
        points.resize(N);

        //qDebug() << "TriangleSurface file read: " << QString::fromStdString(filename);
    }
    else
    {
        //qDebug() << "Could not open file for reading: " << QString::fromStdString(filename);
    }

//    for (int i = 0; i < 5; ++i)
//    {
//        std::cout << points[i].getX() << " " << points[i].getY() << " " << points[i].getZ() << "\n";
//    }
//    std::cout << "\n\n";

//    for (int i = 0; i < points.size() - 1; i +=2)
//    {
//        points.
//    }


//    std::cout << std::setprecision(10) << points.size() << "\n";
}


