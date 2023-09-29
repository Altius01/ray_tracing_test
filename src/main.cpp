#include <iostream>

#include "sphere.h"
#include "projection.h"

#include "Vec3.h"

#define TEST 0

std::ostream &operator<<(std::ostream &stream, const Vec3 &v)
{
    stream << '(' << v.x() << ", " << v.y() << ", " << v.z() << ')';
    return stream;
}

int main()
{
#if TEST
    BoundVec3 volume_position(Vec3(0, 0, 50));
    BoundVec3 lighter_position(Vec3(5, 20, 90));

    FreeVec3 voxel_size(1, 1, 1);

    int nx = 50;
    int ny = 50;
    int nz = 25;

    int px = 1024;
    int py = 1024;

    float rx = 50;
    float ry = 50;

    float radius = 7.f;
#else
    BoundVec3 volume_position(Vec3(30, -10, 50));
    BoundVec3 lighter_position(Vec3(30, 0, 100));

    FreeVec3 voxel_size(0.5, 0.4, 0.3);

    int nx = 1024;
    int ny = 1024;
    int nz = 400;

    int px = 1024;
    int py = 1024;

    float rx = 512;
    float ry = 512;

    float radius = 40.f;
#endif

    float ball_value = 20.0;

    sphere ball(volume_position, voxel_size, nx, ny, nz, radius, ball_value);

    source lighter(lighter_position);

    projection image(px, py, rx, ry);

    image.compute(ball, lighter);

    return 0;
}
