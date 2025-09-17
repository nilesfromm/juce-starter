#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace id
{
#define PARAMETER_ID(str) const juce::ParameterID str (#str, 1) //could be static const?

    PARAMETER_ID (gain1);
    PARAMETER_ID (gain2);
    PARAMETER_ID (gain3);
    PARAMETER_ID (gain4);

    PARAMETER_ID (ratio1);
    PARAMETER_ID (ratio2);
    PARAMETER_ID (ratio3);
    PARAMETER_ID (ratio4);

    PARAMETER_ID (attack);
    PARAMETER_ID (decay);
    PARAMETER_ID (sustain);
    PARAMETER_ID (release);

    PARAMETER_ID (noise);

#undef PARAMETER_ID
} // id

class Parameters
{
public:
    Parameters (juce::AudioProcessorValueTreeState& state);

    [[nodiscard]] static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout (Parameters&);

    void prepareToPlay (double sampleRate) noexcept;
    void reset() noexcept;

    float gain1 = 0.0f;
    float gain2 = 0.0f;
    float gain3 = 0.0f;
    float gain4 = 0.0f;

    float ratio1 = 0.0f;
    float ratio2 = 0.0f;
    float ratio3 = 0.0f;
    float ratio4 = 0.0f;

    float attack = 0.0f;
    float decay = 0.0f;
    float sustain = 0.0f;
    float release = 0.0f;

    float noise = 0.0f;

    juce::AudioParameterFloat* gain1Param;
    juce::AudioParameterFloat* gain2Param;
    juce::AudioParameterFloat* gain3Param;
    juce::AudioParameterFloat* gain4Param;

    juce::AudioParameterFloat* ratio1Param;
    juce::AudioParameterFloat* ratio2Param;
    juce::AudioParameterFloat* ratio3Param;
    juce::AudioParameterFloat* ratio4Param;

    juce::AudioParameterFloat* attackParam;
    juce::AudioParameterFloat* decayParam;
    juce::AudioParameterFloat* sustainParam;
    juce::AudioParameterFloat* releaseParam;

    juce::AudioParameterFloat* noiseParam;

private:
};