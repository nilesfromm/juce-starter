#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace id
{
#define PARAMETER_ID(str) const juce::ParameterID str (#str, 1) //could be static const?

    PARAMETER_ID (h1_ratio);
    PARAMETER_ID (h1_gain);
    PARAMETER_ID (h1_attack);
    PARAMETER_ID (h1_decay);
    PARAMETER_ID (h1_sustain);
    PARAMETER_ID (h1_release);

    PARAMETER_ID (h2_ratio);
    PARAMETER_ID (h2_gain);
    PARAMETER_ID (h2_attack);
    PARAMETER_ID (h2_decay);
    PARAMETER_ID (h2_sustain);
    PARAMETER_ID (h2_release);

    PARAMETER_ID (h3_ratio);
    PARAMETER_ID (h3_gain);
    PARAMETER_ID (h3_attack);
    PARAMETER_ID (h3_decay);
    PARAMETER_ID (h3_sustain);
    PARAMETER_ID (h3_release);

    PARAMETER_ID (h4_ratio);
    PARAMETER_ID (h4_gain);
    PARAMETER_ID (h4_attack);
    PARAMETER_ID (h4_decay);
    PARAMETER_ID (h4_sustain);
    PARAMETER_ID (h4_release);

    PARAMETER_ID (h5_ratio);
    PARAMETER_ID (h5_gain);
    PARAMETER_ID (h5_attack);
    PARAMETER_ID (h5_decay);
    PARAMETER_ID (h5_sustain);
    PARAMETER_ID (h5_release);

    PARAMETER_ID (h6_ratio);
    PARAMETER_ID (h6_gain);
    PARAMETER_ID (h6_attack);
    PARAMETER_ID (h6_decay);
    PARAMETER_ID (h6_sustain);
    PARAMETER_ID (h6_release);

    PARAMETER_ID (h7_ratio);
    PARAMETER_ID (h7_gain);
    PARAMETER_ID (h7_attack);
    PARAMETER_ID (h7_decay);
    PARAMETER_ID (h7_sustain);
    PARAMETER_ID (h7_release);

    PARAMETER_ID (h8_ratio);
    PARAMETER_ID (h8_gain);
    PARAMETER_ID (h8_attack);
    PARAMETER_ID (h8_decay);
    PARAMETER_ID (h8_sustain);
    PARAMETER_ID (h8_release);

    PARAMETER_ID (h9_ratio);
    PARAMETER_ID (h9_gain);
    PARAMETER_ID (h9_attack);
    PARAMETER_ID (h9_decay);
    PARAMETER_ID (h9_sustain);
    PARAMETER_ID (h9_release);

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

    static constexpr int NUM_HARMONICS = 9;

    struct HarmonicParams
    {
        float ratio = 0.0f;
        float gain = 0.0f;
        float attack = 0.0f;
        float decay = 0.0f;
        float sustain = 0.0f;
        float release = 0.0f;
    };

    std::array<HarmonicParams, NUM_HARMONICS> harmonics;

    float noise = 0.0f;

    struct HarmonicParamPointers
    {
        juce::AudioParameterFloat* ratioParam;
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