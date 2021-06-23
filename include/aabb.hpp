#ifndef AABB_H
#define AABB_H

#include <Vector3f.h>
#include "ray.hpp"
#include <cmath>
#include <iostream>
using namespace std;
class AABB
{
public:
	Vector3f _min, _max;
	AABB() : _min(Vector3f::ZERO), _max(Vector3f::ZERO) //最小坐标和最大坐标
	{
	}
	AABB(Vector3f mini, Vector3f maxi) : _min(mini), _max(maxi) //最小坐标和最大坐标
	{
	}

	bool intersect(const Ray &r)
	{
		//xyz 都检查一遍有没有 tmin>= tmax
		auto o = r.origin;
		auto d = r.direction;
		float tmin = -1e6, tmax = 1e6;
		for (int i = 0; i < 3; i++)
		{
			auto t0 = min((_min[i] - o[i]) / d[i],
						  (_max[i] - o[i]) / d[i]);
			auto t1 = max((_min[i] - o[i]) / d[i],
						  (_max[i] - o[i]) / d[i]);
			tmin = max(t0, tmin);
			tmax = min(t1, tmax);
			if (tmax <= tmin)
				return false;
		}
		return true;
	}
};
#endif