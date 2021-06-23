#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

class Triangle : public Object3D
{

public:
	Triangle() = delete;

	// a b c are three vertex positions of the triangle
	Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m) : Object3D(m)
	{
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = a.cross(b - a, b - c).normalized();
	}

	bool intersect(const Ray &ray, Hit &hit, float tmin) override
	{
		auto e1 = vertices[0] - vertices[1];
		auto e2 = vertices[0] - vertices[2];
		auto S = vertices[0] - ray.getOrigin();
		auto rd = ray.getDirection().normalized();
		Matrix3f _divide(rd, e1, e2);
		float divide = _divide.determinant();
		Matrix3f _t(S, e1, e2);
		Matrix3f _b(rd, S, e2);
		Matrix3f _r(rd, e1, S);
		float t = _t.determinant() / divide;
		float b = _b.determinant() / divide;
		float r = _r.determinant() / divide;
		if (t > 0 && b >= 0 && r >= 0 && b <= 1 && r <= 1 && b + r <= 1)
		{
			if (t < tmin || t > hit.getT())
				return false;
			hit.set(t, material, normal);
			return true;
		}
		return false;
	}
	Vector3f normal;
	Vector3f vertices[3];

protected:
};

#endif //TRIANGLE_H
