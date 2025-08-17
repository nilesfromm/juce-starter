#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace id
{
    const juce::ParameterID gain1 { "gain1", 1 };
    const juce::ParameterID gain2 { "gain2", 1 };
    const juce::ParameterID gain3 { "gain3", 1 };
    const juce::ParameterID gain4 { "gain4", 1 };

    const juce::ParameterID ratio1 { "ratio1", 1 };
    const juce::ParameterID ratio2 { "ratio2", 1 };
    const juce::ParameterID ratio3 { "ratio3", 1 };
    const juce::ParameterID ratio4 { "ratio4", 1 };
} // id

class Parameters
{
public:
    Parameters (juce::AudioProcessorValueTreeState& state);

    [[nodiscard]] static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout (Parameters&);

    void prepareToPlay (double sampleRate) noexcept;
    void update() noexcept;
    void reset() noexcept;
    void smooth() noexcept;

    float gain1 = 0.0f;
    float gain2 = 0.0f;
    float gain3 = 0.0f;
    float gain4 = 0.0f;

    float ratio1 = 0.0f;
    float ratio2 = 0.0f;
    float ratio3 = 0.0f;
    float ratio4 = 0.0f;

private:
    juce::AudioParameterFloat* gain1Param;
    juce::LinearSmoothedValue<float> gain1Smoothed;
    juce::AudioParameterFloat* gain2Param;
    juce::LinearSmoothedValue<float> gain2Smoothed;
    juce::AudioParameterFloat* gain3Param;
    juce::LinearSmoothedValue<float> gain3Smoothed;
    juce::AudioParameterFloat* gain4Param;
    juce::LinearSmoothedValue<float> gain4Smoothed;

    juce::AudioParameterFloat* ratio1Param;
    juce::LinearSmoothedValue<float> ratio1Smoothed;
    juce::AudioParameterFloat* ratio2Param;
    juce::LinearSmoothedValue<float> ratio2Smoothed;
    juce::AudioParameterFloat* ratio3Param;
    juce::LinearSmoothedValue<float> ratio3Smoothed;
    juce::AudioParameterFloat* ratio4Param;
    juce::LinearSmoothedValue<float> ratio4Smoothed;
};