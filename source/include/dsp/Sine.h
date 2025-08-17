#pragma once

#include <math.h>
struct Phasor
{
    float phase = 0;
    float phi = 0.0091666;

    void setFreq (float freq, float samplerate) { phi = freq / samplerate; };

    float next()
    {
        phase += phi;
        if (phase > 1.0)
            phase -= 1.0;
        return phase;
    }
};

struct SineOsc
{
    Phasor phasor;
    double sampleRate = 44100;

    void setSampleRate (float samplerate) { sampleRate = samplerate; }

    void setFreq (float freq) { phasor.setFreq (freq, sampleRate); }

    float next() { return sin (2.0 * M_PI * phasor.next()); }
};