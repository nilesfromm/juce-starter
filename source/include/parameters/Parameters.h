#pragma once

#include "ParameterSchema.h"
#include <juce_audio_processors/juce_audio_processors.h>

class Parameters
{
public:
    Parameters (juce::AudioProcessorValueTreeState& state);

    [[nodiscard]] static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay (double sampleRate) noexcept;
    void reset() noexcept;

    std::array<juce::AudioParameterFloat*, TOTAL_PARAMS> paramPointers {};
    juce::AudioParameterFloat* getParam (const char* id) const;

private:
};
