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

    juce::AudioParameterFloat* gain1;
    juce::AudioParameterFloat* gain2;
    juce::AudioParameterFloat* gain3;
    juce::AudioParameterFloat* gain4;

    juce::AudioParameterFloat* ratio1;
    juce::AudioParameterFloat* ratio2;
    juce::AudioParameterFloat* ratio3;
    juce::AudioParameterFloat* ratio4;
};