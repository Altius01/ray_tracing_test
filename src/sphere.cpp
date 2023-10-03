#include "sphere.h"

sphere::sphere(BoundVec3 position, FreeVec3 voxel_size, int nx, int ny, int nz,
               float radius, float ball_value, bool show_progress /*= false*/)
    : volume(position, voxel_size, nx, ny, nz)
{
    this->radius = radius;
    this->ball_value = ball_value;

    fill_voxels(show_progress);
}

float sphere::p(float x, float y, float z)
{
    if (x * x + y * y + z * z <= radius * radius)
        return ball_value;
    else
        return 0.f;
}
