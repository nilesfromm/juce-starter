#pragma once

#include <cstring>

const int NUM_PARAMETERS = 9;

struct Preset
{
    Preset (const char* name, float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8)
    {
        strcpy (this->name, name);
        parameters[0] = p0;
        parameters[1] = p1;
        parameters[2] = p2;
        parameters[3] = p3;
        parameters[4] = p4;
        parameters[5] = p5;
        parameters[6] = p6;
        parameters[7] = p7;
        parameters[8] = p8;
    }

    char name[40];
    float parameters[NUM_PARAMETERS];
};