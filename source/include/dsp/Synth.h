#pragma once

#include "Noise.h"
#include "Voice.h"
#include <array>
#include <cstdint>

class Synth
{
public:
    Synth();
    void allocateResources (double sampleRate, int samplesPerBlock);
    void deallocateResources();
    void reset();
    void render (float** outputBuffers, int sampleCount);
    void midiMessage (uint8_t data0, uint8_t data1, uint8_t data2);
    float noiseMix = 0.0f;

    static constexpr int MAX_VOICES = 8;
    int numVoices = MAX_VOICES;

    float envAttack;
    float envDecay;
    float envSustain;
    float envRelease;

private:
    float sampleRate;
    std::array<Voice, MAX_VOICES> voices;
    void noteOn (int note, int velocity);
    void noteOff (int note);
    void startVoice (int v, int note, int velocity);
    NoiseOsc noiseOsc;
};