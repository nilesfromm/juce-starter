#include "Synth.h"
#include "../utils/Limiter.h"

Synth::Synth()
{
    sampleRate = 44100.0f;
}

void Synth::allocateResources (double sampleRate_, int /*samplesPerBlock*/)
{
    sampleRate = static_cast<float> (sampleRate_);
}

void Synth::deallocateResources()
{
    // TODO: Implement
}

void Synth::reset()
{
    for (int v = 0; v < MAX_VOICES; v++)
    {
        voices[v].reset();
    }
    noiseOsc.reset();
}

void Synth::render (float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];

    for (int sample = 0; sample < sampleCount; sample++)
    {
        float noise = noiseOsc.nextValue() * noiseMix;
        float outputLeft = 0.0f;
        float outputRight = 0.0f;

        for (int v = 0; v < MAX_VOICES; v++)
        {
            Voice& voice = voices[v];
            if (voice.env1.isActive() || voice.env2.isActive() || voice.env3.isActive())
            {
                float output = voice.render (noise);
                outputLeft += output;
                outputRight += output;
            }
        }

        outputBufferLeft[sample] = outputLeft;
        if (outputBufferRight != nullptr)
            outputBufferRight[sample] = outputRight;
    }

    for (int v = 0; v < MAX_VOICES; v++)
    {
        Voice& voice = voices[v];
        if (!voice.env1.isActive() && !voice.env2.isActive() && !voice.env3.isActive())
        {
            voice.env1.reset();
        }
    }

    limiter (outputBufferLeft, sampleCount);
    if (outputBufferRight != nullptr)
        limiter (outputBufferRight, sampleCount);
}

void Synth::midiMessage (uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0)
    {
        // Note off
        case 0x80:
            noteOff (data1 & 0x7F);
            break;
        // Note on
        case 0x90:
            uint8_t note = data1 & 0x7F;
            uint8_t velocity = data2 & 0x7F;
            if (velocity > 0)
                noteOn (note, velocity);
            else
                noteOff (note);
            break;
    }
}

void Synth::noteOn (int note, int velocity)
{
    int v = findFreeVoice();
    startVoice (v, note, velocity);
}

void Synth::startVoice (int v, int note, int velocity)
{
    // float freq = 440.0f * exp2 (float (note - 69) / 12.0f);
    float baseFreq = 8.1758 * exp (0.057762265046662 * float (note));

    Voice& voice = voices[v];
    voice.note = note;
    voice.h1.amp = (velocity / 127.0f) * 0.5f;
    voice.h1.inc = baseFreq / sampleRate;
    voice.h1.reset();

    voice.h2.amp = (velocity / 127.0f) * 0.5f;
    voice.h2.inc = baseFreq * 2.0f / sampleRate;
    voice.h2.reset();

    voice.h3.amp = (velocity / 127.0f) * 0.5f;
    voice.h3.inc = baseFreq * 3.0f / sampleRate;
    voice.h3.reset();

    Envelope& env1 = voice.env1;
    env1.attackMultiplier = h1_attack;
    env1.decayMultiplier = h1_decay;
    env1.sustainLevel = h1_sustain;
    env1.releaseMultiplier = h1_release;
    env1.attack();

    Envelope& env2 = voice.env2;
    env2.attackMultiplier = h2_attack;
    env2.decayMultiplier = h2_decay;
    env2.sustainLevel = h2_sustain;
    env2.releaseMultiplier = h2_release;
    env2.attack();

    Envelope& env3 = voice.env3;
    env3.attackMultiplier = h3_attack;
    env3.decayMultiplier = h3_decay;
    env3.sustainLevel = h3_sustain;
    env3.releaseMultiplier = h3_release;
    env3.attack();
}

void Synth::noteOff (int note)
{
    for (int v = 0; v < MAX_VOICES; v++)
    {
        if (voices[v].note == note)
        {
            voices[v].env1.release();
            voices[v].env2.release();
            voices[v].env3.release();
            voices[v].note = 0;
        }
    }
}

int Synth::findFreeVoice() const
{
    int v = 0;
    float l = 100.0f;

    for (int i = 0; i < MAX_VOICES; i++)
    {
        if (voices[i].env1.level < l && !voices[i].env1.isInAttack())
        {
            l = voices[i].env1.level;
            v = i;
        }
    }
    return v;
}