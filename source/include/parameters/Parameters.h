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

    float h1_gain = 0.0f;
    float h1_attack = 0.0f;
    float h1_decay = 0.0f;
    float h1_sustain = 0.0f;
    float h1_release = 0.0f;

    float h2_gain = 0.0f;
    float h2_attack = 0.0f;
    float h2_decay = 0.0f;
    float h2_sustain = 0.0f;
    float h2_release = 0.0f;

    float h3_gain = 0.0f;
    float h3_attack = 0.0f;
    float h3_decay = 0.0f;
    float h3_sustain = 0.0f;
    float h3_release = 0.0f;

    float h4_gain = 0.0f;
    float h4_attack = 0.0f;
    float h4_decay = 0.0f;
    float h4_sustain = 0.0f;
    float h4_release = 0.0f;

    float noise = 0.0f;

    juce::AudioParameterFloat* ratio1Param;
    juce::AudioParameterFloat* ratio2Param;
    juce::AudioParameterFloat* ratio3Param;
    juce::AudioParameterFloat* ratio4Param;

    juce::AudioParameterFloat* h1_gainParam;
    juce::AudioParameterFloat* h1_attackParam;
    juce::AudioParameterFloat* h1_decayParam;
    juce::AudioParameterFloat* h1_sustainParam;
    juce::AudioParameterFloat* h1_releaseParam;

    juce::AudioParameterFloat* h2_gainParam;
    juce::AudioParameterFloat* h2_attackParam;
    juce::AudioParameterFloat* h2_decayParam;
    juce::AudioParameterFloat* h2_sustainParam;
    juce::AudioParameterFloat* h2_releaseParam;

    juce::AudioParameterFloat* h3_gainParam;
    juce::AudioParameterFloat* h3_attackParam;
    juce::AudioParameterFloat* h3_decayParam;
    juce::AudioParameterFloat* h3_sustainParam;
    juce::AudioParameterFloat* h3_releaseParam;

    juce::AudioParameterFloat* h4_gainParam;
    juce::AudioParameterFloat* h4_attackParam;
    juce::AudioParameterFloat* h4_decayParam;
    juce::AudioParameterFloat* h4_sustainParam;
    juce::AudioParameterFloat* h4_releaseParam;

    juce::AudioParameterFloat* noiseParam;

private:
};