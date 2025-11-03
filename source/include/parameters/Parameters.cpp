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
    for (int i = 0; i < NUM_HARMONICS; ++i)
    {
        harmonics[i].ratio = 1.0f;
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
    return juce::NormalisableRange<float> { 0.f, 1.f, 0.0001f, 1.0f };
}

static juce::NormalisableRange<float> createRatioRange()
{
    return juce::NormalisableRange<float> { 0.f, 12.f, 0.0001f, 1.0f };
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

    // Harmonic envelope parameters
    const juce::ParameterID* harmonicIds[Parameters::NUM_HARMONICS][6] = {
        { &id::h1_ratio, &id::h1_gain, &id::h1_attack, &id::h1_decay, &id::h1_sustain, &id::h1_release },
        { &id::h2_ratio, &id::h2_gain, &id::h2_attack, &id::h2_decay, &id::h2_sustain, &id::h2_release },
        { &id::h3_ratio, &id::h3_gain, &id::h3_attack, &id::h3_decay, &id::h3_sustain, &id::h3_release },
        { &id::h4_ratio, &id::h4_gain, &id::h4_attack, &id::h4_decay, &id::h4_sustain, &id::h4_release },
        { &id::h5_ratio, &id::h5_gain, &id::h5_attack, &id::h5_decay, &id::h5_sustain, &id::h5_release },
        { &id::h6_ratio, &id::h6_gain, &id::h6_attack, &id::h6_decay, &id::h6_sustain, &id::h6_release },
        { &id::h7_ratio, &id::h7_gain, &id::h7_attack, &id::h7_decay, &id::h7_sustain, &id::h7_release },
        { &id::h8_ratio, &id::h8_gain, &id::h8_attack, &id::h8_decay, &id::h8_sustain, &id::h8_release },
        { &id::h9_ratio, &id::h9_gain, &id::h9_attack, &id::h9_decay, &id::h9_sustain, &id::h9_release }
    };

    const char* harmonicNames[Parameters::NUM_HARMONICS][6] = {
        { "h1_ratio", "h1_gain", "h1_attack", "h1_decay", "h1_sustain", "h1_release" },
        { "h2_ratio", "h2_gain", "h2_attack", "h2_decay", "h2_sustain", "h2_release" },
        { "h3_ratio", "h3_gain", "h3_attack", "h3_decay", "h3_sustain", "h3_release" },
        { "h4_ratio", "h4_gain", "h4_attack", "h4_decay", "h4_sustain", "h4_release" },
        { "h5_ratio", "h5_gain", "h5_attack", "h5_decay", "h5_sustain", "h5_release" },
        { "h6_ratio", "h6_gain", "h6_attack", "h6_decay", "h6_sustain", "h6_release" },
        { "h7_ratio", "h7_gain", "h7_attack", "h7_decay", "h7_sustain", "h7_release" },
        { "h8_ratio", "h8_gain", "h8_attack", "h8_decay", "h8_sustain", "h8_release" },
        { "h9_ratio", "h9_gain", "h9_attack", "h9_decay", "h9_sustain", "h9_release" }
    };

    const float ratioDefaults[9] = { 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
    const float harmonicDefaults[6] = { 0.1f, 0.25f, 2.0f, 30.0f, 0.0f, 25.0f };

    for (int h = 0; h < Parameters::NUM_HARMONICS; ++h)
    {
        addParameter (*harmonicIds[h][0], harmonicNames[h][0], createRatioRange(), ratioDefaults[h], parameters.harmonicParams[h].ratioParam, layout);
        addParameter (*harmonicIds[h][1], harmonicNames[h][1], createGainRange(), harmonicDefaults[1], parameters.harmonicParams[h].gainParam, layout);
        addParameter (*harmonicIds[h][2], harmonicNames[h][2], createEnvelopeRange(), harmonicDefaults[2], parameters.harmonicParams[h].attackParam, layout);
        addParameter (*harmonicIds[h][3], harmonicNames[h][3], createEnvelopeRange(), harmonicDefaults[3], parameters.harmonicParams[h].decayParam, layout);
        addParameter (*harmonicIds[h][4], harmonicNames[h][4], createEnvelopeRange(), harmonicDefaults[4], parameters.harmonicParams[h].sustainParam, layout);
        addParameter (*harmonicIds[h][5], harmonicNames[h][5], createEnvelopeRange(), harmonicDefaults[5], parameters.harmonicParams[h].releaseParam, layout);
    }

    // Noise parameter
    addParameter (id::noise, "Noise", juce::NormalisableRange<float> { 0.f, 1.f, 0.01f, 1.0f }, 0.0f, parameters.noiseParam, layout);

    return layout;
}