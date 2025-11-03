#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "melatonin_inspector/melatonin_inspector.h"
#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    // void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    using Resource = juce::WebBrowserComponent::Resource;
    std::optional<Resource> getResource (const juce::String& url) const;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;

    static constexpr int NUM_HARMONICS = 9;

    // Arrays for harmonics (index 0-3 correspond to h1-h4)
    std::array<juce::WebSliderRelay, NUM_HARMONICS> webRatioRelays;
    std::array<juce::WebSliderRelay, NUM_HARMONICS> webGainRelays;
    std::array<juce::WebSliderRelay, NUM_HARMONICS> webAttackRelays;
    std::array<juce::WebSliderRelay, NUM_HARMONICS> webDecayRelays;
    std::array<juce::WebSliderRelay, NUM_HARMONICS> webSustainRelays;
    std::array<juce::WebSliderRelay, NUM_HARMONICS> webReleaseRelays;

    juce::WebSliderRelay webNoiseRelay;

    juce::WebBrowserComponent webView;

    std::array<juce::WebSliderParameterAttachment, NUM_HARMONICS> webRatioAttachments;
    std::array<juce::WebSliderParameterAttachment, NUM_HARMONICS> webGainAttachments;
    std::array<juce::WebSliderParameterAttachment, NUM_HARMONICS> webAttackAttachments;
    std::array<juce::WebSliderParameterAttachment, NUM_HARMONICS> webDecayAttachments;
    std::array<juce::WebSliderParameterAttachment, NUM_HARMONICS> webSustainAttachments;
    std::array<juce::WebSliderParameterAttachment, NUM_HARMONICS> webReleaseAttachments;

    juce::WebSliderParameterAttachment webNoiseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};