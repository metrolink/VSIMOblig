#ifndef OCTAHEDRONBALL_H
#define OCTAHEDRONBALL_H

#include "visualobject.h"
#include <QOpenGLFunctions_4_1_Core>

typedef gsl::Vector3D vec3;
class OctahedronBall : public VisualObject
{
private:
    int m_rekursjoner;
    int m_indeks; // brukes i rekursjon, til å bygge m_vertices
    float mRadius;

    void createTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3);
    void subDivide(const vec3 &a, const vec3 &b, const vec3 &c, int n);
    void octahedronUnitBall();

public:
    OctahedronBall(int n = 3, float radius = 0.25f);
    void init();
    void draw();
};
#endif
