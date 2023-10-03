#include <iostream>

#include <opencv2/highgui.hpp>

#include "sphere.h"
#include "projection.h"

#include "Vec3.h"

#define TEST 0

const std::string WINDOW_NAME = "Raycast demo";

std::ostream &operator<<(std::ostream &stream, const Vec3 &v)
{
    stream << '(' << v.x() << ", " << v.y() << ", " << v.z() << ')';
    return stream;
}

int main()
{
#if TEST
    BoundVec3 volume_position(Vec3(0, 0, 50));
    BoundVec3 lighter_position(Vec3(5, 4, 90));

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
    // Создаем радиус вектора положения центров объема и источника. Задаем координаты через обычный вектор Vec3(x, y, z)
    BoundVec3 volume_position(Vec3(30, -10, 50));
    BoundVec3 lighter_position(Vec3(30, 0, 100));

    // Создаем вектор (не радиус вектор) с размерами вокселя
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

    std::cout << "Положение шарика: " << volume_position
              << "\nПоложение источника: " << lighter_position << std::endl;
    // Создаем объект шар, его положение задем с помощью volume_position
    sphere ball(volume_position, voxel_size, nx, ny, nz, radius, ball_value, true);

    // Создаем объект источник, его положение задем с помощью lighter_position
    source lighter(lighter_position);

    // Создаем проекцию
    projection projection(px, py, rx, ry);

    cv::Mat image(px, py, CV_8UC1);

    cv::namedWindow(WINDOW_NAME, cv::WINDOW_AUTOSIZE);

    projection.compute(image, ball, lighter, true);

    cv::imshow(WINDOW_NAME, image);
    cv::imwrite("image.tiff", image);

    // Ждем нажатия на кнопку, чтобы окно не закрывалось моментально после расчета. TODO: Сделать завершение программы по закртытию окна.
    cv::waitKey(0);
    return 0;
}
