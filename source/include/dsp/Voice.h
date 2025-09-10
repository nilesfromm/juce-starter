#pragma once

#include "Envelope.h"
#include "Oscillator.h"

struct Voice
{
    int note;
    Oscillator osc;
    Envelope env;

    void reset()
    {
        note = 0;
        osc.reset();
        env.reset();
    }

    void release()
    {
        env.release();
    }

    float render (float input)
    {
        float sample = osc.nextSample() + input;
        float envelope = env.nextValue();
        return sample * envelope;
    }
};