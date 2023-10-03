#ifndef SPHERE_H
#define SPHERE_H

#include "volume.h"

class sphere : public volume
{
public:
    float radius;
    float ball_value;

    sphere(BoundVec3 position, FreeVec3 voxel_size, int nx, int ny, int nz,
           float radius, float ball_value, bool show_progress = false);
    float p(float x, float y, float z) override;
};

#endif