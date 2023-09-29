#include "projection.h"

void integrate_voxel(const Ray &ray, const volume &grid, float &integral, int i, int j, int k)
{
    if (i >= grid.numberOfXVoxels() ||
        j >= grid.numberOfYVoxels() ||
        k >= grid.numberOfZVoxels())
    {
        return;
    }
    integral += grid.voxel(i, j, k);
}

std::ostream &
operator<<(std::ostream &stream, const Vec3 &v)
{
    stream << '(' << v.x() << ", " << v.y() << ", " << v.z() << ')';
    return stream;
}

void projection::compute(volume &vol, source &s)
{
    cv::Mat image(px, py, CV_32FC1, cv::Scalar(10.0));

    float step_lenght = MIN(vol.voxelSize().x(), MIN(vol.voxelSize().y(), vol.voxelSize().z()));

    float t0 = 0;

    int counter = 0;

    light_path_int = 0.f;

    for (uint32_t i = 0; i < px; ++i)
    {
        for (uint32_t j = 0; j < py; ++j)
        {
            // Построим луч от источника до центра пикселя проекции
            BoundVec3 pixel_pos = BoundVec3(get_pixel_position(i, j));

            Ray source_ray(s.position, UnitVec3(pixel_pos - s.position));

            float t1 = FreeVec3(pixel_pos - s.position).length();

            float tmin, tmax;
            if (rayBoxIntersection(source_ray, vol, tmin, tmax, t0, t1))
                counter++;
            amanatidesWooAlgorithm(source_ray, vol, t0, t1, integrate_voxel);

            image.at<float>(i, j) = light_path_int;

            if (light_path_int > 300)
                std::cout << i << ' '
                          << j << ' ' << pixel_pos
                          << ' ' << s.position
                          << ' ' << light_path_int << std::endl;

            light_path_int = 0.f;
            // std::cout << light_path_int << std::endl;
        }
    }

    cv::Mat dst;
    cv::normalize(image, dst, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    std::cout << counter << "\nDone!\n";

    BoundVec3 pixel_pos = BoundVec3(Vec3(0.0, 0.0, 0.0));

    Ray source_ray(s.position, UnitVec3(pixel_pos - s.position));

    float t1 = FreeVec3(pixel_pos - s.position).length();

    light_path_int = 0.f;

    float tmin, tmax;

    amanatidesWooAlgorithm(source_ray, vol, t0, t1, integrate_voxel);

    double minVal;
    double maxVal;
    minMaxLoc(image, &minVal, &maxVal);

    std::cout << "min val: " << minVal << std::endl;
    std::cout << "max val: " << maxVal << std::endl;

    std::cout << rayBoxIntersection(source_ray, vol, tmin, tmax, t0, t1) << ' '
              << tmin << ' ' << source_ray.origin() + source_ray.direction() * tmin << ' '
              << tmax << ' ' << source_ray.origin() + source_ray.direction() * tmax << ' '
              << light_path_int << std::endl;
    // cv::namedWindow("Car", cv::WINDOW_AUTOSIZE);

    // cv::imshow("Car", dst);
    cv::imwrite("file.tiff", dst);
    // cv::waitKey(0);
}

FreeVec3 projection::get_pixel_position(int i, int j)
{
    return FreeVec3(((i + 0.5) / px - 0.5) * rx, ((j + 0.5) / py - 0.5) * ry, 0.f);
}

bool projection::rayBoxIntersection(const Ray &ray, const volume &grid, float &tMin, float &tMax,
                                    float t0, float t1) noexcept
{
    float tYMin, tYMax, tZMin, tZMax;
    const float x_inv_dir = 1 / ray.direction().x();
    if (x_inv_dir >= 0)
    {
        tMin = (grid.minBound().x() - ray.origin().x()) * x_inv_dir;
        tMax = (grid.maxBound().x() - ray.origin().x()) * x_inv_dir;
    }
    else
    {
        tMin = (grid.maxBound().x() - ray.origin().x()) * x_inv_dir;
        tMax = (grid.minBound().x() - ray.origin().x()) * x_inv_dir;
    }

    const float y_inv_dir = 1 / ray.direction().y();
    if (y_inv_dir >= 0)
    {
        tYMin = (grid.minBound().y() - ray.origin().y()) * y_inv_dir;
        tYMax = (grid.maxBound().y() - ray.origin().y()) * y_inv_dir;
    }
    else
    {
        tYMin = (grid.maxBound().y() - ray.origin().y()) * y_inv_dir;
        tYMax = (grid.minBound().y() - ray.origin().y()) * y_inv_dir;
    }

    if (tMin > tYMax || tYMin > tMax)
        return false;
    if (tYMin > tMin)
        tMin = tYMin;
    if (tYMax < tMax)
        tMax = tYMax;

    const float z_inv_dir = 1 / ray.direction().z();
    if (z_inv_dir >= 0)
    {
        tZMin = (grid.minBound().z() - ray.origin().z()) * z_inv_dir;
        tZMax = (grid.maxBound().z() - ray.origin().z()) * z_inv_dir;
    }
    else
    {
        tZMin = (grid.maxBound().z() - ray.origin().z()) * z_inv_dir;
        tZMax = (grid.minBound().z() - ray.origin().z()) * z_inv_dir;
    }

    if (tMin > tZMax || tZMin > tMax)
        return false;
    if (tZMin > tMin)
        tMin = tZMin;
    if (tZMax < tMax)
        tMax = tZMax;
    return (tMin < t1 && tMax > t0);
}

void projection::amanatidesWooAlgorithm(const Ray &ray, const volume &grid, float t0, float t1,
                                        std::function<void(const Ray &, const volume &, float &, int, int, int)> func) noexcept
{
    float tMin;
    float tMax;
    const bool ray_intersects_grid = rayBoxIntersection(ray, grid, tMin, tMax, t0, t1);
    if (!ray_intersects_grid)
        return;

    tMin = MAX(tMin, t0);
    tMax = MIN(tMax, t1);
    const BoundVec3 ray_start = ray.origin() + ray.direction() * tMin;
    const BoundVec3 ray_end = ray.origin() + ray.direction() * tMax;

    size_t current_X_index = MAX(0,
                                 std::floor((ray_start.x() - grid.minBound().x()) / grid.voxelSizeX()));
    const size_t end_X_index = MAX(0,
                                   std::floor((ray_end.x() - grid.minBound().x()) / grid.voxelSizeX()));
    int stepX;
    float tDeltaX;
    float tMaxX;
    if (ray.direction().x() > 0.0)
    {
        stepX = 1;
        tDeltaX = grid.voxelSizeX() / ray.direction().x();
        const size_t next_X_index = current_X_index + 1;
        tMaxX = tMin + (grid.minBound().x() +
                        next_X_index * grid.voxelSizeX() - ray_start.x()) /
                           ray.direction().x();
    }
    else if (ray.direction().x() < 0.0)
    {
        stepX = -1;
        tDeltaX = grid.voxelSizeX() / -ray.direction().x();
        tMaxX = tMin + (grid.minBound().x() +
                        current_X_index * grid.voxelSizeX() - ray_start.x()) /
                           ray.direction().x();
    }
    else
    {
        stepX = 0;
        tDeltaX = tMax;
        tMaxX = tMax;
    }

    size_t current_Y_index = MAX(0,
                                 std::floor((ray_start.y() - grid.minBound().y()) / grid.voxelSizeY()));
    const size_t end_Y_index = MAX(0,
                                   std::floor((ray_end.y() - grid.minBound().y()) / grid.voxelSizeY()));
    int stepY;
    float tDeltaY;
    float tMaxY;
    if (ray.direction().y() > 0.0)
    {
        stepY = 1;
        tDeltaY = grid.voxelSizeY() / ray.direction().y();
        const size_t next_Y_index = current_Y_index + 1;
        tMaxY = tMin + (grid.minBound().y() +
                        next_Y_index * grid.voxelSizeY() - ray_start.y()) /
                           ray.direction().y();
    }
    else if (ray.direction().y() < 0.0)
    {
        stepY = -1;
        tDeltaY = grid.voxelSizeY() / -ray.direction().y();
        tMaxY = tMin + (grid.minBound().y() +
                        current_Y_index * grid.voxelSizeY() - ray_start.y()) /
                           ray.direction().y();
    }
    else
    {
        stepY = 0;
        tDeltaY = tMax;
        tMaxY = tMax;
    }

    size_t current_Z_index = MAX(0, std::floor((ray_start.z() - grid.minBound().z()) / grid.voxelSizeZ()));
    const size_t end_Z_index = MAX(0, std::floor((ray_end.z() - grid.minBound().z()) / grid.voxelSizeZ()));
    int stepZ;
    float tDeltaZ;
    float tMaxZ;
    if (ray.direction().z() > 0.0)
    {
        stepZ = 1;
        tDeltaZ = grid.voxelSizeZ() / ray.direction().z();
        const size_t next_Z_index = current_Z_index + 1;
        tMaxZ = tMin + (grid.minBound().z() +
                        next_Z_index * grid.voxelSizeZ() - ray_start.z()) /
                           ray.direction().z();
    }
    else if (ray.direction().z() < 0.0)
    {
        stepZ = -1;
        tDeltaZ = grid.voxelSizeZ() / -ray.direction().z();
        tMaxZ = tMin + (grid.minBound().z() +
                        current_Z_index * grid.voxelSizeZ() - ray_start.z()) /
                           ray.direction().z();
    }
    else
    {
        stepZ = 0;
        tDeltaZ = tMax;
        tMaxZ = tMax;
    }

    // if (current_X_index >= grid.numberOfXVoxels() ||
    //     current_Y_index >= grid.numberOfYVoxels() ||
    //     current_Z_index >= grid.numberOfZVoxels() ||
    //     end_X_index >= grid.numberOfXVoxels() ||
    //     end_Y_index >= grid.numberOfYVoxels() ||
    //     end_Z_index >= grid.numberOfZVoxels())
    // {
    // std::cout << ray_start << ' ' << ray_end << "\n";
    // std::cout << current_X_index << ' ' << end_X_index << '\n'
    //           << current_Y_index << ' ' << end_Y_index << '\n'
    //           << current_Z_index << ' ' << end_Z_index << "\n\n"
    //           << std::endl;
    // }

    float tCurrent = tMin;

    while (current_X_index != end_X_index ||
           current_Y_index != end_Y_index ||
           current_Z_index != end_Z_index)
    {
        float voxel_value = grid.voxel(current_X_index, current_Y_index, current_Z_index);

        if (tMaxX < tMax && tMaxX < tMaxY && tMaxX < tMaxZ)
        {
            // X-axis traversal.
            light_path_int += (tMaxX - tCurrent) * voxel_value;
            current_X_index += stepX;

            tCurrent = tMaxX;
            tMaxX += tDeltaX;
        }
        else if (tMaxY < tMax && tMaxY < tMaxZ)
        {
            // Y-axis traversal.
            light_path_int += (tMaxY - tCurrent) * voxel_value;

            current_Y_index += stepY;

            tCurrent = tMaxY;
            tMaxY += tDeltaY;
        }
        else if (tMaxZ < tMax)
        {
            // Z-axis traversal.
            light_path_int += (tMaxZ - tCurrent) * voxel_value;

            current_Z_index += stepZ;

            tCurrent = tMaxZ;
            tMaxZ += tDeltaZ;
        }
        else
        {
            std::cout << ray_start << ' ' << ray_end << "\n";
            std::cout << tMax << ' ' << tMin << ' ' << tMax
                      << ' ' << tMaxY << ' ' << tMaxZ << '\n';
            std::cout << current_X_index << ' ' << end_X_index << '\n'
                      << current_Y_index << ' ' << end_Y_index << '\n'
                      << current_Z_index << ' ' << end_Z_index << "\n\n"
                      << std::endl;
            break;
        }
    }
}
