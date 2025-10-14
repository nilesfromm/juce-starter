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

    for (int i = 0; i < NUM_HARMONICS; ++i)
    {
        harmonics[i].gain = 0.0f;
        harmonics[i].attack = 0.0f;
        harmonics[i].decay = 0.0f;
        harmonics[i].sustain = 0.0f;
        harmonics[i].release = 0.0f;
    }

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
    return juce::NormalisableRange<float> { 0.f, 1.f, 0.01f, 1.0f };
}

static juce::NormalisableRange<float> createRatioRange()
{
    return juce::NormalisableRange<float> { 0.f, 10.f, 0.01f, 1.0f };
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

    // Harmonic envelope parameters
    const juce::ParameterID* harmonicIds[Parameters::NUM_HARMONICS][5] = {
        { &id::h1_gain, &id::h1_attack, &id::h1_decay, &id::h1_sustain, &id::h1_release },
        { &id::h2_gain, &id::h2_attack, &id::h2_decay, &id::h2_sustain, &id::h2_release },
        { &id::h3_gain, &id::h3_attack, &id::h3_decay, &id::h3_sustain, &id::h3_release },
        { &id::h4_gain, &id::h4_attack, &id::h4_decay, &id::h4_sustain, &id::h4_release }
    };

    const char* harmonicNames[Parameters::NUM_HARMONICS][5] = {
        { "h1_gain", "h1_attack", "h1_decay", "h1_sustain", "h1_release" },
        { "h2_gain", "h2_attack", "h2_decay", "h2_sustain", "h2_release" },
        { "h3_gain", "h3_attack", "h3_decay", "h3_sustain", "h3_release" },
        { "h4_gain", "h4_attack", "h4_decay", "h4_sustain", "h4_release" }
    };

    const float harmonicDefaults[5] = { 0.25f, 2.0f, 30.0f, 0.0f, 25.0f };

    for (int h = 0; h < Parameters::NUM_HARMONICS; ++h)
    {
        addParameter (*harmonicIds[h][0], harmonicNames[h][0], createGainRange(), harmonicDefaults[0], parameters.harmonicParams[h].gainParam, layout);
        addParameter (*harmonicIds[h][1], harmonicNames[h][1], createEnvelopeRange(), harmonicDefaults[1], parameters.harmonicParams[h].attackParam, layout);
        addParameter (*harmonicIds[h][2], harmonicNames[h][2], createEnvelopeRange(), harmonicDefaults[2], parameters.harmonicParams[h].decayParam, layout);
        addParameter (*harmonicIds[h][3], harmonicNames[h][3], createEnvelopeRange(), harmonicDefaults[3], parameters.harmonicParams[h].sustainParam, layout);
        addParameter (*harmonicIds[h][4], harmonicNames[h][4], createEnvelopeRange(), harmonicDefaults[4], parameters.harmonicParams[h].releaseParam, layout);
    }

    // Noise parameter
    addParameter (id::noise, "Noise", juce::NormalisableRange<float> { 0.f, 1.f, 0.01f, 1.0f }, 0.0f, parameters.noiseParam, layout);

    return layout;
}