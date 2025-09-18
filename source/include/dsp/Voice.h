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
    Envelope env4;

    void reset()
    {
        note = 0;
        h1.reset();
        h2.reset();
        h3.reset();
        env1.reset();
        env2.reset();
        env3.reset();
        env4.reset();
    }

    void release()
    {
        env1.release();
        env2.release();
        env3.release();
        env4.release();
    }

    float render (float input, float h1_gain, float h2_gain, float h3_gain, float h4_gain)
    {
        float sample = ((h1.nextSample() * env1.nextValue() * h1_gain) + (h2.nextSample() * env2.nextValue() * h2_gain) + (h3.nextSample() * env3.nextValue() * h3_gain) + (input * env4.nextValue() * h4_gain)) / 4.0f;
        return sample;
    }
};