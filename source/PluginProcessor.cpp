#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (
          BusesProperties()
#if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
              .withInput ("Input", juce::AudioChannelSet::stereo(), true)
    #endif
              .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      parameters (state)
{
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    using namespace juce;

    envelopeFollower.prepare (dsp::ProcessSpec {
        .sampleRate = sampleRate,
        .maximumBlockSize = static_cast<uint32> (samplesPerBlock),
        .numChannels = static_cast<uint32> (getTotalNumOutputChannels()) });
    envelopeFollower.setAttackTime (200.f);
    envelopeFollower.setReleaseTime (200.f);
    envelopeFollower.setLevelCalculationType (
        dsp::BallisticsFilter<float>::LevelCalculationType::peak);

    envelopeFollowerOutputBuffer.setSize (getTotalNumOutputChannels(),
        samplesPerBlock);

    for (int i = 0; i < 4; i++)
        osc[i].setSampleRate (sampleRate);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
#endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    // juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        if (message.isNoteOn())
        {
            // Convert MIDI note number to frequency (A4 = 440Hz)
            float freq = 440.0f * std::pow (2.0f, (message.getNoteNumber() - 69) / 12.0f);
            // Update oscillator 1's frequency
            osc[0].setFreq (freq * parameters.ratio1->get());
            osc[1].setFreq (freq * parameters.ratio2->get());
            osc[2].setFreq (freq * parameters.ratio3->get());
            osc[3].setFreq (freq * parameters.ratio4->get());
        }
    }

    // Get all frequency and amplitude parameters
    // float baseFreq = parameters.frequency->get();
    // float freqs[4] = {
    //     baseFreq, // fundamental
    //     baseFreq * 2.0f, // 1st harmonic (octave)
    //     baseFreq * 3.0f, // 2nd harmonic
    //     baseFreq * 4.0f // 3rd harmonic
    // };

    float amps[4] = {
        parameters.gain1->get(),
        parameters.gain2->get(),
        parameters.gain3->get(),
        parameters.gain4->get()
    };

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    // for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // {
    //     auto* channelData = buffer.getWritePointer (channel);
    //     // juce::ignoreUnused (channelData);
    //     // ..do something to the data...
    //     for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
    //         channelData[sample] = random.nextFloat() * 0.25f - 0.125f;
    // }

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        float sample = 0.0f;

        // Sum all oscillators
        for (int oscIndex = 0; oscIndex < 4; ++oscIndex)
        {
            // Only set frequency for oscillators 2-4 since oscillator 1 is controlled by MIDI
            // if (oscIndex > 0)
            //     osc[oscIndex].setFreq(freqs[oscIndex]);
            // osc[oscIndex].setFreq (freqs[oscIndex]);
            sample += osc[oscIndex].next() * (amps[oscIndex] * 0.5f);
        }

        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);
            channelData[i] = sample;
        }
    }

    // for (int i = 0; i < buffer.getNumSamples(); i++)
    // {
    //     float sample = random.nextFloat() * 0.25f - 0.125f;

    //     for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    //     {
    //         auto* channelData = buffer.getWritePointer (channel);
    //         channelData[i] = sample;
    //     }
    // }

    // *********
    // void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    // {
    //     for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    //     {
    //         // Get a pointer to the start sample in the buffer for this audio output channel
    //         auto* buffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
    //         // Fill the required number of samples with noise between -0.125 and +0.125
    //         for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    //             buffer[sample] = random.nextFloat() * 0.25f - 0.125f;
    //     }
    // }
    // *********

    // if (parameters.bypass->get() || buffer.getNumSamples() == 0)
    // {
    //     return;
    // }

    // juce::dsp::AudioBlock<float> block { buffer };
    // if (parameters.distortionType->getIndex() == 1)
    // {
    //     // tanh(kx)/tanh(k)
    //     juce::dsp::AudioBlock<float>::process (block, block, [] (float sample) {
    //         constexpr auto SATURATION = 5.f;
    //         static const auto normalizationFactor = std::tanh (SATURATION);
    //         sample = std::tanh (SATURATION * sample) / normalizationFactor;
    //         return sample;
    //     });
    // }
    // else if (parameters.distortionType->getIndex() == 2)
    // {
    //     // sigmoid
    //     juce::dsp::AudioBlock<float>::process (block, block, [] (float sample) {
    //         constexpr auto SATURATION = 5.f;
    //         sample = 2.f / (1.f + std::exp (-SATURATION * sample)) - 1.f;
    //         return sample;
    //     });
    // }

    // buffer.applyGain (parameters.gain->get());

    // const auto inBlock =
    //     juce::dsp::AudioBlock<float> { buffer }.getSubsetChannelBlock (
    //         0u, static_cast<size_t> (getTotalNumOutputChannels()));
    // auto outBlock =
    //     juce::dsp::AudioBlock<float> { envelopeFollowerOutputBuffer }.getSubBlock (
    //         0u, static_cast<size_t> (buffer.getNumSamples()));
    // envelopeFollower.process (
    //     juce::dsp::ProcessContextNonReplacing<float> { inBlock, outBlock });
    // outputLevelLeft = juce::Decibels::gainToDecibels (
    //     outBlock.getSample (0, static_cast<int> (outBlock.getNumSamples()) - 1));
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary (*state.copyState().createXml(), destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName (state.state.getType()))
    {
        state.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
