#ifndef BEZIER_H
#define BEZIER_H

#include "object3d.hpp"
#include <vector>
#include "Vector3f.h"
using namespace std;

struct CurvePoint
{
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class BezierCurve
{
public:
    vector<Vector3f> controls;
    int n;

    BezierCurve(double *a, double *b, int _n) : n(_n)
    {
        for (int i = 0; i < n; i++)
            controls.push_back(Vector3f(a[i], b[i], 0));
        n -= 1; //方便索引
    }

    CurvePoint evaluate(double t) const
    { //给一个参数t返回一个点
        CurvePoint ans;
        double *B = new double[n + 1];
        double *dB = new double[n + 1];
        for (int i = 0; i <= n; i++)
        {
            B[i] = 1;
            dB[i] = 1;
        }
        double w1, w2, dw1, dw2;
        for (int p = 1; p <= n; p++)
        {
            for (int i = p; i >= 0; i--)
            {
                if (i == p)
                {
                    B[i] = t * B[i - 1];
                    dB[i] = (t + 1e-3) * dB[i - 1];
                }
                else if (i == 0)
                {
                    B[i] = (1 - t) * B[i];
                    dB[i] = (1 - t - 1e-3) * dB[i];
                }
                else
                {
                    B[i] = (1 - t) * B[i] + t * B[i - 1];
                    dB[i] = (1 - t - 1e-3) * dB[i] + (t + 1e-3) * dB[i - 1];
                }
            }
        }
        ans.V = Vector3f::ZERO;
        ans.T = Vector3f::ZERO;
        for (int i = 0; i <= n; i++)
        {
            ans.V += controls[i] * B[i];
            ans.T += controls[i] * dB[i];
        }
        ans.T = (ans.T - ans.V).normalized();
        delete[] B;
        delete[] dB;
        return ans;
    }
};

double dis_ray(Vector3f point, Ray r)
{
    double t = (point.y() - r.origin.y()) / r.direction.y();
    Vector3f det = r.origin + t * r.direction - point;
    return det.length();
}

class Bezier : public Object3D
{
public:
    Vector3f pos;
    BezierCurve curve;
    Vector3f norm;
    float hmin, hmax, dmax, dmin;
    Bezier(Vector3f p, BezierCurve c) : pos(p), curve(c), norm(Vector3f::ZERO)
    {
        this->material = new Material((0.8, 0.0, 0.5), 2);
        dmin = 1e20, dmax = -1;
        hmin = 1e20, hmax = -1;
        for (double t = 0; t <= 1; t += 1e-3)
        {
            Vector3f cur = curve.evaluate(t).V;
            dmax = max(abs(cur.x()), dmax);
            dmin = min(abs(cur.x()), dmin);
            hmax = max(abs(cur.y()), hmax);
            hmin = min(abs(cur.y()), hmin);
        }
        box = AABB(Vector3f(-dmax + pos.x(), hmin + pos.y(), -dmax + pos.z()), Vector3f(dmax + pos.x(), hmax + pos.y(), dmax + pos.z())); //todo
    }
    //
    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        if (!box.intersect(r))
            return false;
        Vector3f cross = Vector3f::cross(r.direction, Vector3f(0, 1, 0));
        double dis = abs(Vector3f::dot(cross, r.origin - pos)) / cross.length();
        if (dis > dmax)
            return false;
        double t = r.direction.y() >= 0 ? 0 : 1;
        CurvePoint b_point;
        Vector3f center;
        auto Y = r.direction.y();
        double g = 0, dg = 0, rt, eps = 1e-5;
        int cnt = 0;
        if (abs(Y) < eps)
        {
            double target = r.origin.y();
            do
            {
                if (t <= 0 || t > 1)
                    return false;
                b_point = curve.evaluate(t);
                g = b_point.V.y() - target;
                auto next_point = curve.evaluate(t + 1e-3);
                dg = (next_point.V.y() - b_point.V.y()) / 1e-3;
                t = t - 0.5 * g / dg;
            } while (abs(g) >= eps);
            center = Vector3f(0, b_point.V.y(), 0) + pos;
            Vector3f oc = center - r.origin;
            double op = Vector3f::dot(oc, r.direction);
            double cp = sqrt(oc.squaredLength() - op * op);
            if (cp > b_point.V.x())
                return false;
            rt = op - sqrt(b_point.V.x() * b_point.V.x() - cp * cp);
            if (rt >= h.t || rt < tmin)
                return false;
            Vector3f inter = r.origin + rt * r.direction;
            norm = (inter - center).normalized();
            h.set(rt, this->material, norm);
            return true;
        }
        else
        {
            do
            {
                if (++cnt >= 200 || t < 0 || t > 1)
                    return false;
                b_point = curve.evaluate(t);
                double db = abs(b_point.V.x());
                center = Vector3f(0, b_point.V.y(), 0) + pos;
                double dc = dis_ray(center, r);
                g = db - dc;
                auto next_point = curve.evaluate(t + 1e-3);
                double ddb = abs(next_point.V.x() - db);
                double ddc = dis_ray(pos + Vector3f(0, next_point.V.y(), 0), r) - dc;
                dg = (ddb - ddc) / 0.001;
                t = t - .5 * g / dg;
            } while (abs(g) >= eps);
            Vector3f n = Vector3f::cross(b_point.T, Vector3f(0, 0, 1)).normalized();
            Vector3f cv = (b_point.V - center + pos).normalized();
            double no = Vector3f::dot(n, cv);
            Vector3f tg = n - no * cv;
            rt = (center.y() - r.origin.y()) / r.direction.y();
            if (rt >= h.t || rt < tmin)
                return false;
            Vector3f inter = r.origin + rt * r.direction;
            norm = ((inter - center).normalized() * no + tg).normalized();
            h.set(rt, this->material, norm);
            return true;
        }
    }
};
#endif