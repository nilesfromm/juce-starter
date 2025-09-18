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

    float volumeTrim;

    float h1_gain;
    float h1_attack;
    float h1_decay;
    float h1_sustain;
    float h1_release;

    float h2_gain;
    float h2_attack;
    float h2_decay;
    float h2_sustain;
    float h2_release;

    float h3_gain;
    float h3_attack;
    float h3_decay;
    float h3_sustain;
    float h3_release;

    float h4_gain;
    float h4_attack;
    float h4_decay;
    float h4_sustain;
    float h4_release;

private:
    float sampleRate;
    int findFreeVoice() const;
    std::array<Voice, MAX_VOICES> voices;
    void noteOn (int note, int velocity);
    void noteOff (int note);
    void startVoice (int v, int note, int velocity);
    NoiseOsc noiseOsc;
};