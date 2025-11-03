#pragma once

#include "Envelope.h"
#include "Oscillator.h"

struct Voice
{
    int note;
    Oscillator h1;
    Oscillator h2;
    Oscillator h3;
    Oscillator h4;
    Oscillator h5;
    Oscillator h6;
    Oscillator h7;
    Oscillator h8;
    Envelope env1;
    Envelope env2;
    Envelope env3;
    Envelope env4;
    Envelope env5;
    Envelope env6;
    Envelope env7;
    Envelope env8;
    Envelope env9;

    void reset()
    {
        note = 0;
        h1.reset();
        h2.reset();
        h3.reset();
        h4.reset();
        h5.reset();
        h6.reset();
        h7.reset();
        h8.reset();
        env1.reset();
        env2.reset();
        env3.reset();
        env4.reset();
        env5.reset();
        env6.reset();
        env7.reset();
        env8.reset();
        env9.reset();
    }

    void release()
    {
        env1.release();
        env2.release();
        env3.release();
        env4.release();
        env5.release();
        env6.release();
        env7.release();
        env8.release();
        env9.release();
    }

    float render (float input, float h1_gain, float h2_gain, float h3_gain, float h4_gain, float h5_gain, float h6_gain, float h7_gain, float h8_gain, float h9_gain)
    {
        float sample = ((h1.nextSample() * env1.nextValue() * h1_gain) + (h2.nextSample() * env2.nextValue() * h2_gain) + (h3.nextSample() * env3.nextValue() * h3_gain) + (h4.nextSample() * env4.nextValue() * h4_gain) + (h5.nextSample() * env5.nextValue() * h5_gain) + (h6.nextSample() * env6.nextValue() * h6_gain) + (h7.nextSample() * env7.nextValue() * h7_gain) + (h8.nextSample() * env8.nextValue() * h8_gain) + (input * env9.nextValue() * h9_gain)) / 9.0f;
        return sample;
    }
};