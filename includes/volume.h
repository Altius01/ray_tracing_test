#ifndef VOLUME_H
#define VOLUME_H

#include "Vec3.h"

#include <iostream>

class volume
{
private:
    BoundVec3 position_ = BoundVec3(Vec3(0, 0, 0));
    BoundVec3 min_bound_ = BoundVec3(Vec3(0, 0, 0));
    BoundVec3 max_bound_ = BoundVec3(Vec3(0, 0, 0));
    FreeVec3 grid_size_;
    FreeVec3 voxel_size_;
    size_t num_x_voxels_, num_y_voxels_, num_z_voxels_;
    float voxel_size_x_;
    float voxel_size_y_;
    float voxel_size_z_;

    float ***voxels_;

public:
    volume(BoundVec3 position, FreeVec3 voxel_size, int nx, int ny, int nz);
    ~volume()
    {
        delete this->voxels_;
    }

    void fill_voxels(bool show_progress = false);

    inline size_t numberOfXVoxels() const { return num_x_voxels_; }
    inline size_t numberOfYVoxels() const { return num_y_voxels_; }
    inline size_t numberOfZVoxels() const { return num_z_voxels_; }
    inline BoundVec3 minBound() const { return min_bound_; }
    inline BoundVec3 maxBound() const { return max_bound_; }
    inline FreeVec3 gridSize() const { return grid_size_; }
    inline FreeVec3 voxelSize() const { return voxel_size_; }
    inline BoundVec3 position() const { return position_; }
    inline float voxelSizeX() const { return voxel_size_x_; }
    inline float voxelSizeY() const { return voxel_size_y_; }
    inline float voxelSizeZ() const { return voxel_size_z_; }

    inline float voxel(size_t i, size_t j, size_t k) const
    {
        if (i >= this->num_x_voxels_ ||
            j >= this->num_x_voxels_ ||
            k >= this->num_x_voxels_)
            return 0.f;
        else
            return voxels_[i][j][k];
    }

    inline virtual float p(float x, float y, float z)
    {
        return 0.f;
    }

    FreeVec3 get_voxel_position(int i, int j, int k);
};

#endif