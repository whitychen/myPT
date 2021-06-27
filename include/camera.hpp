#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH)
    {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->horizontal.normalize();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
    float fx, fy;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera
{
    double aperture = 0.08; //光圈大小，由于无法做到完美小孔，使得相机光线出现一定波动
    double focus;          //焦距，使得相机射线法相出现波动
public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
                      const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH)
    {
        // angle is in radian.
        fx = (width / 2) / tan(angle / 2);  //单位坐标表示的像素大小
        fy = (height / 2) / tan(angle / 2); //单位坐标表示的像素大小
        focus = (center - Vector3f(-0.5, 0, 1.1)).length();
    }

    Ray generateRay(const Vector2f &point) override
    {
        unsigned short random[3] = {0, 0, point.x() * point.x()};
        unsigned short random2[3] = {0, 0, point.y() * point.y()};
        double dx = erand48(random) * aperture;
        double dy = erand48(random2) * aperture;
        float u = point.x();
        float v = point.y();
        float x = (u - width / 2) / fx;
        float y = (height / 2 - v) / fy;
        Vector3f drc = Vector3f(focus * x - dx, focus * y - dy, focus).normalized();
        // Vector3f drc = Vector3f(x, y, 1).normalized();

        Matrix3f R(horizontal, -up, direction);

        auto dRW = R * drc;
        return Ray(center, dRW.normalized());
    }
};

#endif //CAMERA_H
