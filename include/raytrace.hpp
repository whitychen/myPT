#ifndef raytrace_H
#define raytrace_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include <string>
#include <time.h>

extern unsigned char *texture;
extern int texture_w, texture_h, texture_channl;
Vector3f RayTracing(const Ray &camray, int depth, unsigned short *Xi, Group *mygroup)
{
    double t;
    int id = 0;
    Hit hit;
    if (!mygroup->intersect(camray, hit, 0.001))
        return Vector3f::ZERO;

    Vector3f x = camray.getOrigin() + camray.getDirection() * hit.getT();
    Vector3f n = hit.getNormal();
    Vector3f nl = n.dot(camray.direction, n) < 0 ? n : n * -1;
    Vector3f f = hit.getMaterial()->diffuseColor;
    double p = 0.5;
    auto e = hit.getMaterial()->islight ? hit.getMaterial()->diffuseColor : Vector3f::ZERO;
    Ray reflRay(x, camray.direction - n * 2 * Vector3f::dot(n, camray.direction));
    if (hit.getMaterial()->islight)
        return e;
    if (++depth > 5)
    {
        if (erand48(Xi) < p)
            f = f * (1 / p);
        else
            return e;
    }
    //俄罗斯转盘

    auto shine = hit.getMaterial()->shininess;
    if (shine == 2 || shine == 3) //漫反射
    {
        double r1 = 2 * M_PI * erand48(Xi);
        double r2 = erand48(Xi), r2s = sqrt(r2); //随机一个球
        Vector3f w = nl;
        Vector3f u = Vector3f::cross(fabs(w.x()) > .1 ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0), w).normalized(); 
        Vector3f v = Vector3f::cross(w, u);
        Vector3f d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalized();

        if (shine == 3)
        {
            int idx = ((int)(abs(1000 * x.x())) % texture_w +
                       (int)((abs(x.y() / 2.0)) * texture_h) * texture_w) *
                      texture_channl;
            f = Vector3f(texture[idx + 0], texture[idx + 1], texture[idx + 2]) / 255;
        }

        return e + f * (RayTracing(Ray(x, d), depth, Xi, mygroup)); //颜色*光强
    }

    if (shine == 0) //镜面反射
        return e + f * (RayTracing(reflRay, depth, Xi, mygroup));

    bool into = Vector3f::dot(n, nl) > 0;
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = Vector3f::dot(nl, camray.direction), cos2t; //折射系数
    if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)                                                         //全反射                                                   // 全反射
        return e + f * (RayTracing(reflRay, depth, Xi, mygroup));

    Vector3f tdir = (camray.direction * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).normalized(); //菲涅尔
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : Vector3f::dot(tdir, n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
    return e + f * (depth > 2 ? (erand48(Xi) < P ? // Russian roulette
                                     RayTracing(reflRay, depth, Xi, mygroup) * RP
                                                 : RayTracing(Ray(x, tdir), depth, Xi, mygroup) * TP)
                              : RayTracing(reflRay, depth, Xi, mygroup) * Re + RayTracing(Ray(x, tdir), depth, Xi, mygroup) * Tr);
}

#endif
