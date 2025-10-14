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
    state.state.addListener (this);
    createPresets();
    currentPreset = 0;
}

PluginProcessor::~PluginProcessor()
{
    state.state.removeListener (this);
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
    const int numPresets = presets.size();
    return numPresets > 0 ? numPresets : 1;
}

int PluginProcessor::getCurrentProgram()
{
    return currentPreset;
}

void PluginProcessor::setCurrentProgram (int index)
{
    currentPreset = index;

    juce::RangedAudioParameter* params[NUM_PARAMETERS] = {
        parameters.harmonicParams[0].gainParam,
        parameters.harmonicParams[1].gainParam,
        parameters.harmonicParams[2].gainParam,
        parameters.harmonicParams[3].gainParam,
        parameters.ratio1Param,
        parameters.ratio2Param,
        parameters.ratio3Param,
        parameters.ratio4Param,
        parameters.noiseParam,
    };

    const Preset& preset = presets[index];
    for (int i = 0; i < NUM_PARAMETERS; i++)
    {
        params[i]->setValueNotifyingHost (params[i]->convertTo0to1 (preset.parameters[i]));
    }
    reset();
}

const juce::String PluginProcessor::getProgramName (int index)
{
    return { presets[index].name };
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void PluginProcessor::createPresets()
{
    presets.emplace_back ("Default", 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
    presets.emplace_back ("Number 2", 0.5f, 0.25f, 0.25f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 0.66f);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.allocateResources (sampleRate, samplesPerBlock);
    parameters.prepareToPlay (sampleRate);
    parametersChanged.store (true);
    reset();

    // for (int i = 0; i < 4; i++)
    //     osc[i].setSampleRate (sampleRate);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    synth.deallocateResources();
}

void PluginProcessor::reset()
{
    parameters.reset();
    synth.reset();
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

void PluginProcessor::updateParameters()
{
    float sampleRate = float (getSampleRate());
    float inverseSampleRate = 1.0f / sampleRate;

    for (int h = 0; h < Parameters::NUM_HARMONICS; ++h)
    {
        synth.harmonics[h].attack = std::exp (-inverseSampleRate * std::exp (5.5f - 0.075f * parameters.harmonicParams[h].attackParam->get()));
        synth.harmonics[h].decay = std::exp (-inverseSampleRate * std::exp (5.5f - 0.075f * parameters.harmonicParams[h].decayParam->get()));
        synth.harmonics[h].sustain = parameters.harmonicParams[h].sustainParam->get() / 100.0f;

        float envRelease = parameters.harmonicParams[h].releaseParam->get();
        if (envRelease < 1.0f)
        {
            synth.harmonics[h].release = 0.75f;
        }
        else
        {
            synth.harmonics[h].release = std::exp (-inverseSampleRate * std::exp (5.5f - 0.075f * envRelease));
        }
    }

    float noiseMix = parameters.noiseParam->get();
    noiseMix *= noiseMix;
    synth.noiseMix = noiseMix * 0.06f;

    for (int h = 0; h < Parameters::NUM_HARMONICS; ++h)
    {
        synth.harmonics[h].gain = parameters.harmonicParams[h].gainParam->get();
    }
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

    bool expected = true;
    if (isNonRealtime() || parametersChanged.compare_exchange_strong (expected, false))
    {
        updateParameters();
    }

    splitBufferByEvents (buffer, midiMessages);
    // Process MIDI messages
    // for (const auto metadata : midiMessages)
    // {
    //     const auto message = metadata.getMessage();
    //     if (message.isNoteOn())
    //     {
    //         // Convert MIDI note number to frequency (A4 = 440Hz)
    //         baseFreq = 440.0f * std::pow (2.0f, (message.getNoteNumber() - 69) / 12.0f);
    //     }
    // }

    // Get all frequency and amplitude parameters
    // parameters.update();

    // float freqs[4] = {
    //     baseFreq * parameters.ratio1,
    //     baseFreq * parameters.ratio2,
    //     baseFreq * parameters.ratio3,
    //     baseFreq * parameters.ratio4
    // };

    // float amps[4] = {
    //     parameters.gain1,
    //     parameters.gain2,
    //     parameters.gain3,
    //     parameters.gain4
    // };

    // Set all oscillator frequencies
    // for (int i = 0; i < 4; i++)
    // {
    //     osc[i].setFreq (freqs[i]);
    // }

    // float* channelDataL = buffer.getWritePointer (0);
    // float* channelDataR = buffer.getWritePointer (1);

    // for (int i = 0; i < buffer.getNumSamples(); i++)
    // {
    //     parameters.smooth();
    //     float sample = 0.0f;

    //     osc[0].setFreq (baseFreq * parameters.ratio1);
    //     osc[1].setFreq (baseFreq * parameters.ratio2);
    //     osc[2].setFreq (baseFreq * parameters.ratio3);
    //     osc[3].setFreq (baseFreq * parameters.ratio4);

    //     sample += osc[0].next() * (parameters.gain1 * 0.5f);
    //     sample += osc[1].next() * (parameters.gain2 * 0.5f);
    //     sample += osc[2].next() * (parameters.gain3 * 0.5f);
    //     sample += osc[3].next() * (parameters.gain4 * 0.5f);

    //     // Sum all oscillators
    //     // for (int oscIndex = 0; oscIndex < 4; ++oscIndex)
    //     // {
    //     //     sample += osc[oscIndex].next() * (amps[oscIndex] * 0.5f);
    //     // }

    //     channelDataL[i] = sample;
    //     channelDataR[i] = sample;
    // }
}

void PluginProcessor::splitBufferByEvents (juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    int bufferOffset = 0;

    for (const auto metadata : midiMessages)
    {
        // Render the audio that happens before this event (if any).
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0)
        {
            render (buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }

        // Handle the event. Ignore MIDI messages such as sysex.
        if (metadata.numBytes <= 3)
        {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI (metadata.data[0], data1, data2);
        }
    }

    // Render the audio after the last MIDI event. If there were no
    // MIDI events at all, this renders the entire buffer.
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0)
    {
        render (buffer, samplesLastSegment, bufferOffset);
    }

    midiMessages.clear();
}

void PluginProcessor::handleMIDI (uint8_t data0, uint8_t data1, uint8_t data2)
{
    // Program Change
    if ((data0 & 0xF0) == 0xC0)
    {
        if (data1 < presets.size())
        {
            setCurrentProgram (data1);
        }
    }

    synth.midiMessage (data0, data1, data2);
    // char s[16];
    // snprintf (s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    // DBG (s);
}

void PluginProcessor::render (
    juce::AudioBuffer<float>& buffer,
    int sampleCount,
    int bufferOffset)
{
    float* outputBuffers[2] = { nullptr, nullptr };
    outputBuffers[0] = buffer.getWritePointer (0) + bufferOffset;
    if (getTotalNumOutputChannels() > 1)
    {
        outputBuffers[1] = buffer.getWritePointer (1) + bufferOffset;
    }
    synth.render (outputBuffers, sampleCount);
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
