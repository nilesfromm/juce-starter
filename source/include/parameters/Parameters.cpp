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
    ratio1 = 0.0f;
    ratio2 = 0.0f;
    ratio3 = 0.0f;
    ratio4 = 0.0f;

    h1_gain = 0.0f;
    h1_attack = 0.0f;
    h1_decay = 0.0f;
    h1_sustain = 0.0f;
    h1_release = 0.0f;

    h2_gain = 0.0f;
    h2_attack = 0.0f;
    h2_decay = 0.0f;
    h2_sustain = 0.0f;
    h2_release = 0.0f;

    h3_gain = 0.0f;
    h3_attack = 0.0f;
    h3_decay = 0.0f;
    h3_sustain = 0.0f;
    h3_release = 0.0f;

    h4_gain = 0.0f;
    h4_attack = 0.0f;
    h4_decay = 0.0f;
    h4_sustain = 0.0f;
    h4_release = 0.0f;

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

    // Ratio parameters
    addParameter (id::ratio1, "ratio1", createRatioRange(), 0.5f, parameters.ratio1Param, layout);
    addParameter (id::ratio2, "ratio2", createRatioRange(), 1.0f, parameters.ratio2Param, layout);
    addParameter (id::ratio3, "ratio3", createRatioRange(), 2.0f, parameters.ratio3Param, layout);
    addParameter (id::ratio4, "ratio4", createRatioRange(), 4.0f, parameters.ratio4Param, layout);

    // Envelope parameters
    addParameter (id::h1_gain, "h1_gain", createGainRange(), 0.25f, parameters.h1_gainParam, layout);
    addParameter (id::h1_attack, "h1_attack", createEnvelopeRange(), 2.0f, parameters.h1_attackParam, layout);
    addParameter (id::h1_decay, "h1_decay", createEnvelopeRange(), 30.0f, parameters.h1_decayParam, layout);
    addParameter (id::h1_sustain, "h1_sustain", createEnvelopeRange(), 0.0f, parameters.h1_sustainParam, layout);
    addParameter (id::h1_release, "h1_release", createEnvelopeRange(), 25.0f, parameters.h1_releaseParam, layout);

    addParameter (id::h2_gain, "h2_gain", createGainRange(), 0.25f, parameters.h2_gainParam, layout);
    addParameter (id::h2_attack, "h2_attack", createEnvelopeRange(), 2.0f, parameters.h2_attackParam, layout);
    addParameter (id::h2_decay, "h2_decay", createEnvelopeRange(), 30.0f, parameters.h2_decayParam, layout);
    addParameter (id::h2_sustain, "h2_sustain", createEnvelopeRange(), 0.0f, parameters.h2_sustainParam, layout);
    addParameter (id::h2_release, "h2_release", createEnvelopeRange(), 25.0f, parameters.h2_releaseParam, layout);

    addParameter (id::h3_gain, "h3_gain", createGainRange(), 0.25f, parameters.h3_gainParam, layout);
    addParameter (id::h3_attack, "h3_attack", createEnvelopeRange(), 2.0f, parameters.h3_attackParam, layout);
    addParameter (id::h3_decay, "h3_decay", createEnvelopeRange(), 30.0f, parameters.h3_decayParam, layout);
    addParameter (id::h3_sustain, "h3_sustain", createEnvelopeRange(), 0.0f, parameters.h3_sustainParam, layout);
    addParameter (id::h3_release, "h3_release", createEnvelopeRange(), 25.0f, parameters.h3_releaseParam, layout);

    addParameter (id::h4_gain, "h4_gain", createGainRange(), 0.25f, parameters.h4_gainParam, layout);
    addParameter (id::h4_attack, "h4_attack", createEnvelopeRange(), 2.0f, parameters.h4_attackParam, layout);
    addParameter (id::h4_decay, "h4_decay", createEnvelopeRange(), 30.0f, parameters.h4_decayParam, layout);
    addParameter (id::h4_sustain, "h4_sustain", createEnvelopeRange(), 0.0f, parameters.h4_sustainParam, layout);
    addParameter (id::h4_release, "h4_release", createEnvelopeRange(), 25.0f, parameters.h4_releaseParam, layout);

    // Noise parameter
    addParameter (id::noise, "Noise", juce::NormalisableRange<float> { 0.f, 1.f, 0.01f, 1.0f }, 0.0f, parameters.noiseParam, layout);

    return layout;
}