#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>

// todo Implement Group - add data structure to store a list of Object*
class Group : public Object3D
{

public:
    Group()
    {
    }

    explicit Group(int num_objects)
    {
        // while (group.size() < num_objects)
        //     group.push_back()
    }

    ~Group() override
    {
        for (auto it = group.begin(); it != group.end();)
        {
            // delete *it;//?
            it = group.erase(it);
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        bool flag = false;
        for (auto it = group.begin(); it != group.end(); it++)
        {
            flag = (*it)->intersect(r, h, tmin) ? true : flag;
        }
        return flag;
    }

    void addObject(int index, Object3D *obj)
    {
        group.push_back(obj);
    }

    int getGroupSize()
    {
        return group.size();
    }


    std::vector<Object3D *> group;
};

#endif
