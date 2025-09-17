#include "Parameters.h"
#include "dsp/Synth.h"

Parameters::Parameters (juce::AudioProcessorValueTreeState& state)
{
}

void Parameters::prepareToPlay (double sampleRate) noexcept
{
    double duration = 0.02f;
}

void Parameters::reset() noexcept
{
    gain1 = 0.0f;
    gain2 = 0.0f;
    gain3 = 0.0f;
    gain4 = 0.0f;

    ratio1 = 0.0f;
    ratio2 = 0.0f;
    ratio3 = 0.0f;
    ratio4 = 0.0f;

    attack = 0.0f;
    decay = 0.0f;
    sustain = 0.0f;
    release = 0.0f;

    noise = 0.0f;
}

static void addParameter (
    juce::ParameterID id,
    const juce::String& name,
    const juce::NormalisableRange<float>& range,
    float defaultValue,
    juce::AudioParameterFloat*& parameterPointer,
    juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    auto parameter = std::make_unique<juce::AudioParameterFloat> (
        id, name, range, defaultValue);
    parameterPointer = parameter.get();
    layout.add (std::move (parameter));
}

static juce::NormalisableRange<float> createGainRange()
{
    return juce::NormalisableRange<float> { 0.f, 1.f, 0.01f, 0.9f };
}

static juce::NormalisableRange<float> createRatioRange()
{
    return juce::NormalisableRange<float> { 0.f, 10.f, 0.01f, 0.9f };
}

static juce::NormalisableRange<float> createEnvelopeRange()
{
    return juce::NormalisableRange<float> { 0.f, 100.f, 1.0f, 1.0f };
}

juce::AudioProcessorValueTreeState::ParameterLayout
    Parameters::createParameterLayout (
        Parameters& parameters)
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    // Gain parameters
    addParameter (id::gain1, "gain1", createGainRange(), 0.25f, parameters.gain1Param, layout);
    addParameter (id::gain2, "gain2", createGainRange(), 0.25f, parameters.gain2Param, layout);
    addParameter (id::gain3, "gain3", createGainRange(), 0.25f, parameters.gain3Param, layout);
    addParameter (id::gain4, "gain4", createGainRange(), 0.25f, parameters.gain4Param, layout);

    // Ratio parameters
    addParameter (id::ratio1, "ratio1", createRatioRange(), 0.5f, parameters.ratio1Param, layout);
    addParameter (id::ratio2, "ratio2", createRatioRange(), 1.0f, parameters.ratio2Param, layout);
    addParameter (id::ratio3, "ratio3", createRatioRange(), 2.0f, parameters.ratio3Param, layout);
    addParameter (id::ratio4, "ratio4", createRatioRange(), 4.0f, parameters.ratio4Param, layout);

    // Envelope parameters
    addParameter (id::attack, "Attack", createEnvelopeRange(), 2.0f, parameters.attackParam, layout);
    addParameter (id::decay, "Decay", createEnvelopeRange(), 30.0f, parameters.decayParam, layout);
    addParameter (id::sustain, "Sustain", createEnvelopeRange(), 0.0f, parameters.sustainParam, layout);
    addParameter (id::release, "Release", createEnvelopeRange(), 25.0f, parameters.releaseParam, layout);

    // Noise parameter
    addParameter (id::noise, "Noise", juce::NormalisableRange<float> { 0.f, 1.f, 0.01f, 1.0f }, 0.0f, parameters.noiseParam, layout);

    return layout;
}