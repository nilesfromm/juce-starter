#pragma once

#include "Envelope.h"
#include "Oscillator.h"

struct Voice
{
    int note;
    Oscillator h1;
    Oscillator h2;
    Oscillator h3;
    Envelope env1;
    Envelope env2;
    Envelope env3;

    void reset()
    {
        note = 0;
        h1.reset();
        h2.reset();
        h3.reset();
        env1.reset();
        env2.reset();
        env3.reset();
    }

    void release()
    {
        env1.release();
        env2.release();
        env3.release();
    }

    float render (float input)
    {
        float sample = ((h1.nextSample() * env1.nextValue()) + (h2.nextSample() * env2.nextValue()) + (h3.nextSample() * env3.nextValue())) / 4.0f + input;
        return sample;
    }
};