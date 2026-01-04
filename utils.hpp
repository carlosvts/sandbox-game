#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"


enum TYPES 
{
    VOID = 0,
    SAND,
    WATER,
    FIRE,
    IRON,
    GAS,
};


struct Particle
{
    TYPES type; 
    Rectangle rect;
    float velocity {0};
    float acceleration {0};
    Color color;
    bool exists = false;
    bool wasUpdated = false;
};


#endif
