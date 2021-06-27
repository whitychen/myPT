// #ifndef STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #endif

#ifndef TEXTURE_H
#define TEXTURE_H

#include "stb_image.hpp"
#include <string>
#include "Vector3f.h"
using namespace std;

class texture
{
    // private:
public:
    unsigned char *_texture;
    int texture_w, texture_h, texture_channl;

    texture(/* args */){

    };
    texture(const char *filename)
    {
        _texture = stbi_load(filename, &texture_w, &texture_h, &texture_channl, 0);
    };
    Vector3f getcolor(int x, int y)
    {
        int idx = ((int)(abs(x)) % texture_w +
                   (int)(abs(y)) % texture_h * texture_w) *
                  texture_channl;
        return Vector3f(_texture[idx + 0], _texture[idx + 1], _texture[idx + 2]) / 255;
    }
    ~texture(){
      delete []_texture;  
    };
};

#endif