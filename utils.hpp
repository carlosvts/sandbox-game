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
    LAVA,
    CONWAY,
};


struct Particle
{
    TYPES type; 
    Rectangle rect;
    Color color;
    bool exists = false;
    bool wasUpdated = false;
};


#endif
