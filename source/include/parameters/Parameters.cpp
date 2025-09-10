#include "Parameters.h"
#include "dsp/Synth.h"

Parameters::Parameters (juce::AudioProcessorValueTreeState& state)
{
}

void Parameters::prepareToPlay (double sampleRate) noexcept
{
    double duration = 0.02f;
    gain1Smoothed.reset (sampleRate, duration);
    gain2Smoothed.reset (sampleRate, duration);
    gain3Smoothed.reset (sampleRate, duration);
    gain4Smoothed.reset (sampleRate, duration);

    ratio1Smoothed.reset (sampleRate, duration);
    ratio2Smoothed.reset (sampleRate, duration);
    ratio3Smoothed.reset (sampleRate, duration);
    ratio4Smoothed.reset (sampleRate, duration);

    attackSmoothed.reset (sampleRate, duration);
    decaySmoothed.reset (sampleRate, duration);
    sustainSmoothed.reset (sampleRate, duration);
    releaseSmoothed.reset (sampleRate, duration);

    noiseSmoothed.reset (sampleRate, duration);
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

    gain1Smoothed.setCurrentAndTargetValue (gain1Param->get());
    gain2Smoothed.setCurrentAndTargetValue (gain2Param->get());
    gain3Smoothed.setCurrentAndTargetValue (gain3Param->get());
    gain4Smoothed.setCurrentAndTargetValue (gain4Param->get());

    ratio1Smoothed.setCurrentAndTargetValue (ratio1Param->get());
    ratio2Smoothed.setCurrentAndTargetValue (ratio2Param->get());
    ratio3Smoothed.setCurrentAndTargetValue (ratio3Param->get());
    ratio4Smoothed.setCurrentAndTargetValue (ratio4Param->get());

    attackSmoothed.setCurrentAndTargetValue (attackParam->get());
    decaySmoothed.setCurrentAndTargetValue (decayParam->get());
    sustainSmoothed.setCurrentAndTargetValue (sustainParam->get());
    releaseSmoothed.setCurrentAndTargetValue (releaseParam->get());

    noiseSmoothed.setCurrentAndTargetValue (noiseParam->get());
}

void Parameters::update() noexcept
{
    gain1Smoothed.setTargetValue (gain1Param->get());
    gain2Smoothed.setTargetValue (gain2Param->get());
    gain3Smoothed.setTargetValue (gain3Param->get());
    gain4Smoothed.setTargetValue (gain4Param->get());

    ratio1Smoothed.setTargetValue (ratio1Param->get());
    ratio2Smoothed.setTargetValue (ratio2Param->get());
    ratio3Smoothed.setTargetValue (ratio3Param->get());
    ratio4Smoothed.setTargetValue (ratio4Param->get());

    attackSmoothed.setTargetValue (attackParam->get());
    decaySmoothed.setTargetValue (decayParam->get());
    sustainSmoothed.setTargetValue (sustainParam->get());
    releaseSmoothed.setTargetValue (releaseParam->get());

    noiseSmoothed.setTargetValue (noiseParam->get());
}

void Parameters::smooth() noexcept
{
    gain1 = gain1Smoothed.getNextValue();
    gain2 = gain2Smoothed.getNextValue();
    gain3 = gain3Smoothed.getNextValue();
    gain4 = gain4Smoothed.getNextValue();

    ratio1 = ratio1Smoothed.getNextValue();
    ratio2 = ratio2Smoothed.getNextValue();
    ratio3 = ratio3Smoothed.getNextValue();
    ratio4 = ratio4Smoothed.getNextValue();

    attack = attackSmoothed.getNextValue();
    decay = decaySmoothed.getNextValue();
    sustain = sustainSmoothed.getNextValue();
    release = releaseSmoothed.getNextValue();
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
    addParameter (id::attack, "Attack", createEnvelopeRange(), 0.0f, parameters.attackParam, layout);
    addParameter (id::decay, "Decay", createEnvelopeRange(), 30.0f, parameters.decayParam, layout);
    addParameter (id::sustain, "Sustain", createEnvelopeRange(), 0.0f, parameters.sustainParam, layout);
    addParameter (id::release, "Release", createEnvelopeRange(), 25.0f, parameters.releaseParam, layout);

    // Noise parameter
    addParameter (id::noise, "Noise", juce::NormalisableRange<float> { 0.f, 1.f, 0.01f, 1.0f }, 0.0f, parameters.noiseParam, layout);

    return layout;
}