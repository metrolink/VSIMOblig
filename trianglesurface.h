#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H

#include "visualobject.h"

class TriangleSurface : public VisualObject
{
public:
    TriangleSurface();
    TriangleSurface(std::string filename);
    ~TriangleSurface() override;

    virtual void init() override;
    virtual void draw() override;

    void readFile(std::string filename);
    void writeFile(std::string filename);
    void construct();
    void createSurface();
    std::vector<gsl::Vector3D> getTrianglePoints();
};

#endif //TRIANGLESURFACE_H
