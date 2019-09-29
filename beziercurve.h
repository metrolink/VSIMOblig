#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H
#include "visualobject.h"

typedef gsl::Vector3D vec3;
typedef gsl::Vector4D vec4;
class BezierCurve : public VisualObject
{
public:
    BezierCurve(std::vector<vec3> controlPoints, int degrees);
    virtual void init();
    virtual void draw();
    vec3 Bezier(float t, int degree);

    std::vector<vec3> getPoints();

    std::vector<vec3> getControl() const;

    void setColor(float r, float g, float b);

private:
    std::vector<vec3> b;
    std::vector<vec3> points;
    std::vector<vec3> control;
};

#endif // BEZIERCURVE_H
