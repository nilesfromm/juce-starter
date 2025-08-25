#pragma once

#include "include/dsp/Sine.h"
#include "include/dsp/Synth.h"
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
    void reset() override;

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
    float baseFreq = 440.0f;

    // Midi ======================================================================
    void splitBufferByEvents (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void handleMIDI (uint8_t data0, uint8_t data1, uint8_t data2);
    void render (juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);

    // Parameters ==================================================================
    juce::AudioProcessorValueTreeState state {
        *this,
        nullptr,
        "Parameters",
        Parameters::createParameterLayout (parameters)
    };
    Parameters parameters;

    // Custom DSP ================================================================
    Synth synth;
    SineOsc osc[4];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};