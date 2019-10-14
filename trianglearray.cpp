#include "trianglearray.h"
#include "matrix4x4.h"
#include <cmath>
#include <fstream>

TriangleArray::TriangleArray() {
}

// Backwards compatibility to glDrawArrays
TriangleArray::TriangleArray(Triangle vertices) {
    this->push_back(vertices);
}
// Backwards compatibility to glDrawArrays
TriangleArray::TriangleArray(std::vector<Triangle> triangles) {
    for (Triangle tri : triangles) {
        this->push_back(tri);
    }
}
TriangleArray::TriangleArray(std::vector<Vertex> vertices) {
    if (vertices.size() % 3 != 0) {
        throw std::out_of_range("Vertex vector is not a multiple of three, something went wrong (Not enough vertices to make sufficient triangles.");
    } else {
        for (unsigned int i = 0; i < vertices.size() - 1; i += 3) {
            Triangle tri(vertices.at(i), vertices.at(i + 1), vertices.at(i + 2));
            this->push_back(tri);
        }
    }
}
TriangleArray::TriangleArray(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->push_back(vertices, indices);
}
TriangleArray::~TriangleArray() {
}
/**
 * @brief TriangleArray::push_back Add another triangle to the array.
 * Backwards compatibility function, if you already have indices then pass them in as well
 * @param vertices
 */
void TriangleArray::push_back(Triangle triangle) {
    mTriangles.push_back(triangle);
    checkForDuplicates(triangle);
    // Should probably just change function to take tuple
}
void TriangleArray::push_back(std::vector<Vertex> vertices) {
    for (unsigned int i = 0; i < vertices.size() - 1; i += 3) {
        Triangle tri(vertices.at(i), vertices.at(i + 1), vertices.at(i + 2));
        mTriangles.push_back(tri);
        checkForDuplicates(tri);
    }
}
void TriangleArray::push_back(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    // separate into triangles and check if vertices have dupes
    // if they do, swap the index of that vertex with the other index.
    mVertices = vertices;
    mIndices = indices;
    for (unsigned int i{0}; i < indices.size() - 2; i += 3) {
        Triangle tri(mVertices.at(mIndices.at(i)), mVertices.at(mIndices.at(i + 1)), mVertices.at(mIndices.at(i + 2)));
        mTriangles.push_back(tri);
    }
    if (indices.size() % 3 != 0)
        throw std::out_of_range("3 indices required per triangle!");
}
void TriangleArray::checkForDuplicates(Triangle triangle) {
    tempIndex.resize(3);
    duplicate(triangle.vert[0], 0);
    duplicate(triangle.vert[1], 1);
    duplicate(triangle.vert[2], 2);

    for (unsigned int i = 0; i < tempIndex.size(); i++) {
        mIndices.push_back(tempIndex.at(i));
    }
    tempIndex.clear();
}
/**
 * @brief TriangleArray::duplicate Checks if a vertex already exists in the mVertices vector
 * @param vert
 * @param index Denotes which point of the triangle it's checking -
 * just makes it easier to keep track of what part of tempIndex I'm changing
 * @return
 */
bool TriangleArray::duplicate(Vertex vert, int index) {
    if (mVertices.size() != 0) {
        for (unsigned int i = 0; i < mVertices.size(); i++) // for each vertex
        {
            if (mVertices.at(i) == vert) // if vertex at i is same as in-vertex, return true (found a duplicate)
            {
                tempIndex.at(index) = i;
                return true;
            }
        }
    }
    // if we get this far, no dupes were found
    mVertices.push_back(vert);
    tempIndex.at(index) = mVertices.size() - 1; // size of the vector is the last element number
    return false;                               // didn't find any duplicates, you may add this to mVertices
}

/**
 * @brief TriangleArray::calcVertexNormals Recalculate each vertex normal according to its neighbours with a weighted average.
 */
void TriangleArray::calcVertexNormals() {
    for (unsigned int i = 0; i < mVertices.size(); i++) {
        vec3 normal = vec3();
        Vertex newVert = mVertices[i];
        for (Triangle triangle : mTriangles) {
            if (triangle.hasVertex(newVert)) {
                normal += triangle.normal; // add triangle normal to total normal for averaging out later
            }
        }
        normal.normalize(); // normalize to get the averaged normal
        newVert.set_normal(normal);
        mSmoothVertices.push_back(newVert);
    }
}
float TriangleArray::angleBetweenSurfaceNormals(const Triangle &tri1, const Triangle &tri2) {
    float dot = tri1.normal * tri2.normal;
    float magnitude = tri1.normal.length() * tri2.normal.length();
    float radAngle = dot / magnitude;
    return gsl::rad2deg(acosf(radAngle));
}
bool TriangleArray::getIsWeighted() const {
    return mIsWeighted;
}

std::vector<Vertex> TriangleArray::getSmoothVertices() {
    if (mSmoothVertices.empty())
        calcVertexNormals();
    return mSmoothVertices;
}

void TriangleArray::setVertices(const std::vector<Vertex> &vertices) {
    mVertices = vertices;
}

void TriangleArray::setIndices(const std::vector<unsigned int> &indices) {
    mIndices = indices;
}
/**
 * @brief TriangleArray::at
 * @param index
 * @return Returns a vector of three Vec3 points constituting one triangle.
 * Easier than getting stuff from the tuple itself.
 */
std::vector<vec3> TriangleArray::at(unsigned int index) const {
    std::vector<vec3> triangleVector;
    triangleVector.push_back(mTriangles.at(index).vert[0].XYZ());
    triangleVector.push_back(mTriangles.at(index).vert[1].XYZ());
    triangleVector.push_back(mTriangles.at(index).vert[2].XYZ());
    return triangleVector;
}
/**
 * @brief TriangleArray::atW
 * @param index
 * @return Returns a vector of three Vec3 points same as at(index) but adjusted for world coordinates.
 */
std::vector<vec3> TriangleArray::atW(unsigned int index, const gsl::Matrix4x4 &mMatrix) {
    std::vector<vec3> triangleVector;
    triangleVector.push_back(mTriangles.at(index).vert[0].XYZ());
    triangleVector.push_back(mTriangles.at(index).vert[1].XYZ());
    triangleVector.push_back(mTriangles.at(index).vert[2].XYZ());
    for (vec3 &point : triangleVector) {
        vec4 point4d = vec4(point, 1);
        point = (mMatrix * point4d).getXYZ();
    }
    return triangleVector;
}
