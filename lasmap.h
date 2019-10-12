#ifndef LASMAP_H
#define LASMAP_H

#include "visualobject.h"
#include "LASLoader.h"

class LasMap/* : public LASLoader*/
{
public:
    LasMap();
    //~LasMap() override;

//    virtual void init() override;
//    virtual void draw() override;

    void readFile(std::string filename);


};

#endif // LASMAP_H
