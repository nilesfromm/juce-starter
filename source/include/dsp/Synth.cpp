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
            if (voice.env.isActive())
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
        if (!voice.env.isActive())
        {
            voice.env.reset();
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
    startVoice (0, note, velocity);
}

void Synth::startVoice (int v, int note, int velocity)
{
    // float freq = 440.0f * exp2 (float (note - 69) / 12.0f);
    float freq = 8.1758 * exp (0.057762265046662 * float (note));

    Voice& voice = voices[v];
    voice.note = note;
    voice.osc.amp = (velocity / 127.0f) * 0.5f;
    voice.osc.inc = freq / sampleRate;
    voice.osc.reset();

    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
    env.attack();
}

void Synth::noteOff (int note)
{
    Voice& voice = voices[0];
    if (voice.note == note)
    {
        voice.env.release();
    }
}