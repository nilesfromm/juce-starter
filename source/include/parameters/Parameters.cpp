#include "Parameters.h"

Parameters::Parameters (juce::AudioProcessorValueTreeState& state)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
    Parameters::createParameterLayout (
        Parameters& parameters)
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::gain1, "gain1", NormalisableRange<float> { 0.f, 1.f, 0.01f, 0.9f }, 0.5f);
        parameters.gain1 = parameter.get();
        layout.add (std::move (parameter));
    }

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::gain2, "gain2", NormalisableRange<float> { 0.f, 1.f, 0.01f, 0.9f }, 0.5f);
        parameters.gain2 = parameter.get();
        layout.add (std::move (parameter));
    }

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::gain3, "gain3", NormalisableRange<float> { 0.f, 1.f, 0.01f, 0.9f }, 0.5f);
        parameters.gain3 = parameter.get();
        layout.add (std::move (parameter));
    }

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::gain4, "gain4", NormalisableRange<float> { 0.f, 1.f, 0.01f, 0.9f }, 0.5f);
        parameters.gain4 = parameter.get();
        layout.add (std::move (parameter));
    }

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::ratio1, "ratio1", NormalisableRange<float> { 0.f, 10.f, 0.01f, 0.9f }, 0.5f);
        parameters.ratio1 = parameter.get();
        layout.add (std::move (parameter));
    }

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::ratio2, "ratio2", NormalisableRange<float> { 0.f, 10.f, 0.01f, 0.9f }, 1.0f);
        parameters.ratio2 = parameter.get();
        layout.add (std::move (parameter));
    }

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::ratio3, "ratio3", NormalisableRange<float> { 0.f, 10.f, 0.01f, 0.9f }, 2.0f);
        parameters.ratio3 = parameter.get();
        layout.add (std::move (parameter));
    }

    {
        auto parameter = std::make_unique<AudioParameterFloat> (
            id::ratio4, "ratio4", NormalisableRange<float> { 0.f, 10.f, 0.01f, 0.9f }, 4.0f);
        parameters.ratio4 = parameter.get();
        layout.add (std::move (parameter));
    }

    return layout;
}