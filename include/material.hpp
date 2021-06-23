#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

class Material
{
public:
    explicit Material(const Vector3f &d_color, float s = 0, float r = 0, bool l = false) : diffuseColor(d_color), shininess(s), reflective(r), islight(l)
    {
    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const
    {
        return diffuseColor;
    }

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor)
    {
        int tern = 1;
        Vector3f shaded = Vector3f::ZERO;
        if (islight)
        {
            shaded = diffuseColor;
        }
        else
        {
            for (int i = 0; i < tern; i++)
            {
                auto li = dirToLight.normalized();
                auto n_li = Vector3f::dot(hit.getNormal(), li);
                float k = n_li > 0 ? n_li : 0;
                shaded += diffuseColor * k;
            }
            shaded = shaded / tern;
        }
        return shaded;
    }

public:
    Vector3f diffuseColor; //颜色
    float shininess;       //反射率
    float reflective;      //折射率
    bool islight;
};

#endif // MATERIAL_H
