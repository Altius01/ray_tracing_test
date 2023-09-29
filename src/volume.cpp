#include "volume.h"

volume::volume(BoundVec3 position, FreeVec3 voxel_size, int nx, int ny, int nz)
{
    this->position_ = BoundVec3(position);
    this->voxel_size_ = voxel_size;
    this->voxel_size_x_ = voxel_size.x();
    this->voxel_size_y_ = voxel_size.y();
    this->voxel_size_z_ = voxel_size.z();
    this->num_x_voxels_ = nx;
    this->num_y_voxels_ = ny;
    this->num_z_voxels_ = nz;
    this->grid_size_ = voxel_size.hadamar(FreeVec3(nx, ny, nz));

    this->max_bound_ = BoundVec3(position_ + grid_size_ * 0.5);
    this->min_bound_ = BoundVec3(position_ - grid_size_ * 0.5);
}

void volume::fill_voxels()
{
    voxels_ = new float **[num_x_voxels_];

    for (size_t i = 0; i < num_x_voxels_; ++i)
    {
        voxels_[i] = new float *[num_y_voxels_];
        for (size_t j = 0; j < num_y_voxels_; ++j)
        {
            voxels_[i][j] = new float[num_z_voxels_];
            for (size_t k = 0; k < num_z_voxels_; ++k)
            {
                FreeVec3 voxel_pos;
                voxel_pos = get_voxel_position(i, j, k);

                voxels_[i][j][k] = p(voxel_pos.x(), voxel_pos.y(), voxel_pos.z());
            }
        }
    }
}

FreeVec3 volume::get_voxel_position(int i, int j, int k)
{
    // Получим положение центра вокселя, относительно края вокселя [0][0][0] и нормируем на число вокселей
    FreeVec3 delta((i + 0.5), (j + 0.5), (k + 0.5));

    // получим положение вокселя относительно центра объемя, для этого вычтем вектор (0.5, 0.5, 0.5) т.е. перейдем в другую С.К.
    delta = delta - FreeVec3(0.5 * num_x_voxels_, 0.5 * num_y_voxels_, 0.5 * num_z_voxels_);
    return voxel_size_.hadamar(delta);
}