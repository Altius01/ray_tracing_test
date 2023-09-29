#include "source.h"

source::source(BoundVec3 position)
{
    this->position = position;
}

source::source(float x, float y, float z)
{
    position = BoundVec3(Vec3(x, y, z));
}
