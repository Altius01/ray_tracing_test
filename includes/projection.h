#ifndef PROJECTION_H
#define PROJECTION_H

#include <iostream>
#include <functional>

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>

#include "Vec3.h"
#include "source.h"
#include "volume.h"

class projection
{
public:
    int px, py;
    float rx, ry;

    float light_path_int;
    FreeVec3 position;
    projection(int px, int py, float rx, float ry)
    {
        this->px = px;
        this->py = py;

        this->rx = rx;
        this->ry = ry;
    }
    ~projection() {}

    FreeVec3 get_pixel_position(int i, int j);
    void compute(volume &v, source &s);

    bool rayBoxIntersection(const Ray &ray, const volume &grid, float &tMin, float &tMax,
                            float t0, float t1) noexcept;
    void amanatidesWooAlgorithm(const Ray &ray, const volume &grid, float t0, float t1,
                                std::function<void(const Ray &, const volume &, float &, int, int, int)> func) noexcept;
};

#endif