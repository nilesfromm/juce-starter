#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace id
{
#define PARAMETER_ID(str) const juce::ParameterID str (#str, 1) //could be static const?

    PARAMETER_ID (ratio1);
    PARAMETER_ID (ratio2);
    PARAMETER_ID (ratio3);
    PARAMETER_ID (ratio4);

    PARAMETER_ID (h1_gain);
    PARAMETER_ID (h1_attack);
    PARAMETER_ID (h1_decay);
    PARAMETER_ID (h1_sustain);
    PARAMETER_ID (h1_release);

    PARAMETER_ID (h2_gain);
    PARAMETER_ID (h2_attack);
    PARAMETER_ID (h2_decay);
    PARAMETER_ID (h2_sustain);
    PARAMETER_ID (h2_release);

    PARAMETER_ID (h3_gain);
    PARAMETER_ID (h3_attack);
    PARAMETER_ID (h3_decay);
    PARAMETER_ID (h3_sustain);
    PARAMETER_ID (h3_release);

    PARAMETER_ID (h4_gain);
    PARAMETER_ID (h4_attack);
    PARAMETER_ID (h4_decay);
    PARAMETER_ID (h4_sustain);
    PARAMETER_ID (h4_release);

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

    float ratio1 = 0.0f;
    float ratio2 = 0.0f;
    float ratio3 = 0.0f;
    float ratio4 = 0.0f;

    static constexpr int NUM_HARMONICS = 4;

    struct HarmonicParams
    {
        float gain = 0.0f;
        float attack = 0.0f;
        float decay = 0.0f;
        float sustain = 0.0f;
        float release = 0.0f;
    };

    std::array<HarmonicParams, NUM_HARMONICS> harmonics;

    float noise = 0.0f;

    juce::AudioParameterFloat* ratio1Param;
    juce::AudioParameterFloat* ratio2Param;
    juce::AudioParameterFloat* ratio3Param;
    juce::AudioParameterFloat* ratio4Param;

    struct HarmonicParamPointers
    {
        juce::AudioParameterFloat* gainParam;
        juce::AudioParameterFloat* attackParam;
        juce::AudioParameterFloat* decayParam;
        juce::AudioParameterFloat* sustainParam;
        juce::AudioParameterFloat* releaseParam;
    };

    std::array<HarmonicParamPointers, NUM_HARMONICS> harmonicParams;

    juce::AudioParameterFloat* noiseParam;

private:
};