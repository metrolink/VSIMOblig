#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H
#include "Objects/visualobject.h"
#include "vector.h"

class GameObject;
typedef Vector3D vec3;
class BezierCurve : public VisualObject
{
public:
    BezierCurve(std::vector<vec3> controlPoints, int degrees);
    virtual void init(Shader *shader);
    virtual void draw();
    vec3 Bezier(float t, int degree);

    GameObject *getObject();

    std::vector<vec3> getPoints();

    std::vector<vec3> getControl() const;

    void move(const vec3 &loc);

    void setColor(float r, float g, float b);

private:
    std::vector<vec3> b;
    std::vector<vec3> points;
    std::vector<vec3> control;
};

#endif // BEZIERCURVE_H
