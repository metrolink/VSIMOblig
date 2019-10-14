#ifndef TRIANGLEARRAY_H
#define TRIANGLEARRAY_H
#include "gsl_math.h"
#include "texture.h"
#include "vertex.h"
#include <vector>

typedef gsl::Vector3D vec3;
typedef gsl::Vector4D vec4;
struct Triangle {
    Vertex vert[3];
    vec3 normal;
    Triangle(Vertex p1,
             Vertex p2,
             Vertex p3)
        : vert{p1, p2, p3} {
        calcNormal();
    }
    void operator()(Vertex a, Vertex b, Vertex c) {
        vert[0] = a;
        vert[1] = b;
        vert[2] = c;
        calcNormal();
    }

    bool operator==(const Triangle &tri) {
        return vert[0] == tri.vert[0] && vert[1] == tri.vert[1] && vert[2] == tri.vert[2];
    }
    void calcNormal() {
        vec3 edge1 = vert[1].XYZ() - vert[0].XYZ();
        vec3 edge2 = vert[2].XYZ() - vert[0].XYZ();
        normal = (edge1 ^ edge2).normalized();
    }
    bool hasVertex(const Vertex &vertex) {
        vec3 vertCoords[3]{vert[0].XYZ(), vert[1].XYZ(), vert[2].XYZ()};
        vec3 vertexCoords = vertex.XYZ();
        return vertCoords[0] == vertexCoords || vertCoords[1] == vertexCoords || vertCoords[2] == vertexCoords;
    }
};

class TriangleArray {
public:
    TriangleArray();
    // Using tuple to prevent more than 3 vertices in case of a mistake
    TriangleArray(Triangle vertices);
    TriangleArray(std::vector<Triangle> vertices);
    TriangleArray(std::vector<Vertex> vertices);
    TriangleArray(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    ~TriangleArray();

    void push_back(Triangle vertices);
    void push_back(std::vector<Vertex> vertices);
    void push_back(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    std::vector<vec3> at(unsigned int index) const;
    std::vector<vec3> atW(unsigned int index, const gsl::Matrix4x4 &mMatrix);

    void switchNormals();
    void calcVertexNormals();

    /**
     * @brief TriangleArray::size
     * @return How many triangles are in the array
     */
    unsigned int size() { return mTriangles.size(); }
    std::vector<Vertex> getVertices() { return mVertices; }

    /**
     * @brief TriangleArray::getTriPoints
     * @return Returns the tuple if you absolutely must have it for some reason.
     */
    std::vector<Triangle> getTriangles() const { return mTriangles; }

    std::vector<unsigned int> getIndices() const { return mIndices; }

    bool getIsWeighted() const;

    void sharedVertices();

    std::vector<Vertex> getSmoothVertices();
    void setVertices(const std::vector<Vertex> &vertices);
    void setIndices(const std::vector<unsigned int> &indices);

private:
    std::vector<Triangle> mTriangles;

    std::vector<Vertex> mVertices;
    std::vector<Vertex> mSmoothVertices;

    std::vector<unsigned int> mIndices;
    std::vector<unsigned int> tempIndex;

    std::vector<Texture> mTextures;

    bool mIsWeighted{false};
    bool calculated{false};

    bool duplicate(Vertex vert, int index);
    void checkForDuplicates(Triangle vertices);
    float angleBetweenSurfaceNormals(const Triangle &tri1, const Triangle &tri2);
};

#endif // TRIANGLEARRAY_H
