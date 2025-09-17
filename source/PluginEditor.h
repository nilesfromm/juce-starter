#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "melatonin_inspector/melatonin_inspector.h"
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

    juce::WebSliderRelay webGain1Relay;
    juce::WebSliderRelay webGain2Relay;
    juce::WebSliderRelay webGain3Relay;
    juce::WebSliderRelay webGain4Relay;
    juce::WebSliderRelay webRatio1Relay;
    juce::WebSliderRelay webRatio2Relay;
    juce::WebSliderRelay webRatio3Relay;
    juce::WebSliderRelay webRatio4Relay;

    juce::WebSliderRelay webA1Relay;
    juce::WebSliderRelay webD1Relay;
    juce::WebSliderRelay webS1Relay;
    juce::WebSliderRelay webR1Relay;

    juce::WebSliderRelay webA2Relay;
    juce::WebSliderRelay webD2Relay;
    juce::WebSliderRelay webS2Relay;
    juce::WebSliderRelay webR2Relay;

    juce::WebSliderRelay webA3Relay;
    juce::WebSliderRelay webD3Relay;
    juce::WebSliderRelay webS3Relay;
    juce::WebSliderRelay webR3Relay;

    juce::WebSliderRelay webNoiseRelay;

    juce::WebBrowserComponent webView;

    juce::WebSliderParameterAttachment webGain1SliderAttachment;
    juce::WebSliderParameterAttachment webGain2SliderAttachment;
    juce::WebSliderParameterAttachment webGain3SliderAttachment;
    juce::WebSliderParameterAttachment webGain4SliderAttachment;
    juce::WebSliderParameterAttachment webRatio1SliderAttachment;
    juce::WebSliderParameterAttachment webRatio2SliderAttachment;
    juce::WebSliderParameterAttachment webRatio3SliderAttachment;
    juce::WebSliderParameterAttachment webRatio4SliderAttachment;

    juce::WebSliderParameterAttachment webA1Attachment;
    juce::WebSliderParameterAttachment webD1Attachment;
    juce::WebSliderParameterAttachment webS1Attachment;
    juce::WebSliderParameterAttachment webR1Attachment;

    juce::WebSliderParameterAttachment webA2Attachment;
    juce::WebSliderParameterAttachment webD2Attachment;
    juce::WebSliderParameterAttachment webS2Attachment;
    juce::WebSliderParameterAttachment webR2Attachment;

    juce::WebSliderParameterAttachment webA3Attachment;
    juce::WebSliderParameterAttachment webD3Attachment;
    juce::WebSliderParameterAttachment webS3Attachment;
    juce::WebSliderParameterAttachment webR3Attachment;

    juce::WebSliderParameterAttachment webNoiseSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};