#ifndef _COMMON_H
#define _COMMON_H

#include<GL/glew.h>
#include <windows.h>
#include <Winuser.h>
#include<iostream>
#include <algorithm>
#include<stdlib.h>
#include<vector>
#include<gl/gl.h>
#include<GL/glut.h>
#include<time.h>
#include<math.h>
#include<string>
#include <memory>
#include<time.h>
#include<fstream>
#include<map>
#define PI 3.14159
#define GRAVITY 9.81
using namespace std;
enum WeaponType { DEFAULT, ROCKET, LASER, FLAK, ENERGY_FIELD };
enum EnemyType { SWARMBOT, BUGSHIP, SMALLBUG, BOSSSHIP };

struct AABB {
    float minX, minY, maxX, maxY;
};

struct vec3
{
    float x,y,z;

    // Default constructor: initialize to zero
    vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    // Parameterized constructor
    vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

};

struct vec2
{
    float x,y;

    // Default constructor: initialize to zero
    vec2() : x(0.0f), y(0.0f) {}

    // Parameterized constructor
    vec2(float x_, float y_) : x(x_), y(y_) {}

};

struct vec4
{
    float x,y,z,w;

    // Default constructor: initialize to zero
    vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

    // Parameterized constructor
    vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
};
#endif // _COMMON_H
