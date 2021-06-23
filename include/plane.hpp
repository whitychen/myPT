#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D
{
public:
    Plane()
    {
    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m), normal(normal) // noraml * p +D =0
    {
        D = -d;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        float t = -(D + normal.dot(normal, r.getOrigin())) / normal.dot(normal, r.getDirection().normalized());
        if (t > 0 && t > tmin && t < h.getT())
        {
            h.set(t, material, normal);
            return true;
        }
        return false;
    }

protected:
    float D;
    Vector3f normal;
};

#endif //PLANE_H
