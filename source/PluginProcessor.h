#pragma once

#include "include/dsp/Sine.h"
#include "include/parameters/Parameters.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#if (MSVC)
    #include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    [[nodiscard]] juce::AudioProcessorValueTreeState& getState() noexcept
    {
        return state;
    }

    std::atomic<float> outputLevelLeft;

private:
    juce::dsp::BallisticsFilter<float> envelopeFollower;
    juce::AudioBuffer<float> envelopeFollowerOutputBuffer;
    juce::Random random;

    // Parameters ==================================================================
    juce::AudioProcessorValueTreeState state {
        *this,
        nullptr,
        "Parameters",
        Parameters::createParameterLayout (parameters)
    };
    Parameters parameters;

    // Custom DSP ================================================================
    SineOsc osc[4];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};