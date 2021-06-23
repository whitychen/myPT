#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Sphere : public Object3D
{
public:
    Sphere()
    {
        // unit ball at the center
        center = Vector3f(0, 0, 0);
        radius = 1.0;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material), center(center), radius(radius)
    {
        auto tem = Vector3f(radius, radius, radius);
        box = AABB(center - tem, center + tem);
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        if (!box.intersect(r))
            return false;
        //求交
        //合法更新hit
        Vector3f Sc = center - r.getOrigin();
        float loc_square = Sc.squaredLength();
        float tca = Sc.dot(Sc, r.getDirection().normalized());
        float thc_suqare = radius * radius - loc_square + tca * tca;
        if (loc_square < radius)
        { // 内部点
            float t = tca + sqrt(thc_suqare);
            if (t < tmin)
                return false;
            if (t <= h.getT())
            {
                Vector3f cross = r.getOrigin() + r.getDirection().normalized() * t;
                Vector3f n = (cross - center) / radius;
                h.set(t, material, n.normalized());
                return true;
            }
        }
        else //外部点
        {
            if (tca < 0)
                return false;
            if (thc_suqare < 0)
                return false;
            float thc = sqrt(thc_suqare);
            float t = tca - thc;
            if (t < tmin)
                return false;
            if (t <= h.getT())
            {
                Vector3f cross = r.getOrigin() + r.getDirection().normalized() * t;
                Vector3f n = (cross - center) / radius;
                h.set(t, material, n.normalized());
                return true;
            }
        }
        return false;
    }

protected:
    float radius;
    Vector3f center;
};

#endif
