#ifndef LASMAP_H
#define LASMAP_H

#include "visualobject.h"
#include "LASLoader.h"

class LasMap : public VisualObject
{
public:
    LasMap();
    ~LasMap() override;

    virtual void init() override;
    virtual void draw() override;

    gsl::LASLoader lasloader{"../VSIMOblig/LASdata/33-1-497-327-21.las"};
    void printSomePoints();
    void addAllPointsToVertices();

    void centerMap();

    //"../VSIMOblig/LASdata/33-1-497-327-20.las"
};

#endif // LASMAP_H
