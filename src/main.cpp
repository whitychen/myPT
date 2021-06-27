#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <queue>
#include <iostream>

#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "raytrace.hpp"
#include "bezier.hpp"
#include <string>
#include <time.h>

using namespace std;

inline double clamp(double x) { return x < 0 ? 0 : x > 1 ? 1
                                                         : x; }

unsigned char *texture;
int texture_w, texture_h, texture_channl;
int main(int argc, char *argv[])
{
    texture = stbi_load("texture/vase.png", &texture_w, &texture_h, &texture_channl, 0);
    // std::cerr << texture_w << " " << texture_h << " " << texture_channl << std::endl;

    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3)
    {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    auto inputFile = argv[1];
    auto outputFile = argv[2]; // only bmp is allowed.

    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    SceneParser myscene(inputFile);
    Camera *mycamera = myscene.getCamera();
    Vector3f background = myscene.getBackgroundColor();
    Group *mygroup = myscene.getGroup();
    Image output(mycamera->getWidth(), mycamera->getHeight());

    double a[4] = {0.3, 0.4, 0.1, 0.2};
    double b[4] = {0, 0.4, 0.5, 0.9};
    BezierCurve vasecurve(a, b, 4);
    Object3D *vase = new Bezier(Vector3f(0.5, 0, -0.5), vasecurve);
    mygroup->addObject(0, vase);

    Object3D *vase2 = new Bezier(Vector3f(-0.5, 0, 1.0), vasecurve);
    mygroup->addObject(0, vase2);

    int samps = 100;

    int h = mycamera->getHeight();
    int w = mycamera->getWidth();
    Ray cam(mycamera->center, mycamera->direction);
    Vector3f cx = mycamera->horizontal;
    Vector3f cy = mycamera->up;
    Vector3f r = Vector3f::ZERO;
    Vector3f *c = new Vector3f[w * h];

#ifndef _OPENMP
    std::cerr << "no openmp!\n";
#endif

#pragma omp parallel for schedule(dynamic, 1) private(r) // OpenMP
    for (int y = 0; y < h; ++y)
    {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100. * y / (h - 1));
        unsigned short Xi[3] = {0, 0, y * y * y};

        for (int x = 0; x < w; ++x)
        {
            for (int sy = 0, i = (h - y - 1) * w + x; sy < 2; sy++) // 2x2 subpixel rows
                for (int sx = 0; sx < 2; sx++, r = Vector3f::ZERO)
                { // 2x2 subpixel cols
                    for (int s = 0; s < samps; s++)
                    {
                        double r1 = 2 * erand48(Xi), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double r2 = 2 * erand48(Xi), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        Vector3f d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
                                     cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.getDirection();
                        //加入随机因素抗锯齿
                        auto camray = mycamera->generateRay(Vector2f((sx + .5 + dx) / 2 + x, (sy + .5 + dy) / 2 + y));
                        r = r + RayTracing(camray, 0, Xi, mygroup) * (1. / samps);
                    }
                    c[i] = c[i] + Vector3f(clamp(r.x()), clamp(r.y()), clamp(r.z())) * .25;
                }
        }
    }
    for (int i = 0; i < w * h; i++)
    {
        auto x = i % w;
        auto y = h - 1 + (x - i) / w;
        output.SetPixel(x, y, c[i]);
    }
    output.SaveImage(outputFile);
    cout << "Hello! Computer Graphics!" << endl;
    return 0;
}
