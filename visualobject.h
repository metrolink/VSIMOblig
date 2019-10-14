#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include "matrix4x4.h"
#include "shader.h"
#include "trianglearray.h"
#include "vertex.h"
#include <QOpenGLFunctions_4_1_Core>
#include <vector>
class RenderWindow;
class VisualObject : public QOpenGLFunctions_4_1_Core {
public:
    VisualObject(TriangleArray *triangles = nullptr);
    virtual ~VisualObject();
    virtual void init();
    virtual void draw() = 0;

    std::string mName;

    RenderWindow *mRenderWindow; //Just to be able to call checkForGLerrors()
    virtual void move(gsl::Vector3D vec);
    gsl::Matrix4x4 getModelMatrix();

    bool getUseTextures() const;
    void setUseTextures(bool value);

    void scale(gsl::Vector3D vec);
    void rotate(gsl::Vector3D vec);

    gsl::Vector3D getPosition();

protected:
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    gsl::Matrix4x4 mMatrix;
    gsl::Matrix4x4 mTransMatrix, mRotMatrix, mScaleMatrix;
    TriangleArray *mObjectTriangles{nullptr};
    bool useTextures{false};
    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)
};
#endif // VISUALOBJECT_H
