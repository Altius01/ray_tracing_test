#ifndef SOURCE_H
#define SOURCE_H

#include "Vec3.h"

class source
{
public:
    BoundVec3 position = BoundVec3(Vec3(0, 0, 0));

    source(BoundVec3 position);
    source(float x, float y, float z);
    ~source() {}
};

#endif
