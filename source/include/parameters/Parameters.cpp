#include "Parameters.h"
#include <cstring>

Parameters::Parameters (juce::AudioProcessorValueTreeState& state)
{
    for (int i = 0; i < TOTAL_PARAMS; ++i)
    {
        paramPointers[i] = dynamic_cast<juce::AudioParameterFloat*> (
            state.getParameter (PARAM_SCHEMA[i].id));
    }
}

void Parameters::prepareToPlay (double sampleRate) noexcept
{
}

void Parameters::reset() noexcept
{
}

juce::AudioParameterFloat* Parameters::getParam (const char* id) const
{
    for (int i = 0; i < TOTAL_PARAMS; ++i)
    {
        if (std::strcmp (PARAM_SCHEMA[i].id, id) == 0)
            return paramPointers[i];
    }
    return nullptr;
}

juce::AudioProcessorValueTreeState::ParameterLayout
    Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& p : PARAM_SCHEMA)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { p.id, 1 },
            p.id,
            juce::NormalisableRange<float> { p.min, p.max, p.step, p.skew },
            p.defaultVal));
    }

    return layout;
}
