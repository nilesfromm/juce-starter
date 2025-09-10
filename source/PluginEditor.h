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
    void nativeFunction (
        const juce::Array<juce::var>& args,
        juce::WebBrowserComponent::NativeFunctionCompletion completion);

    juce::TextButton runJavaScriptButton { "Run some JavaScript" };
    juce::TextButton emitJavaScriptEventButton { "Emit JavaScript event" };
    juce::Label labelUpdatedFromJavaScript { "label",
        "To be updated from JavaScript" };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    juce::Label gainLabel { "gain label", "Gain" };
    juce::Slider gain1Slider { "gain1 slider" };
    juce::Slider gain2Slider { "gain2 slider" };
    juce::Slider gain3Slider { "gain3 slider" };
    juce::Slider gain4Slider { "gain4 slider" };
    juce::SliderParameterAttachment gain1SliderAttachment;
    juce::SliderParameterAttachment gain2SliderAttachment;
    juce::SliderParameterAttachment gain3SliderAttachment;
    juce::SliderParameterAttachment gain4SliderAttachment;

    juce::Label ratioLabel { "ratio label", "Ratio" };
    juce::Slider ratio1Slider { "ratio1 slider" };
    juce::Slider ratio2Slider { "ratio2 slider" };
    juce::Slider ratio3Slider { "ratio3 slider" };
    juce::Slider ratio4Slider { "ratio4 slider" };
    juce::SliderParameterAttachment ratio1SliderAttachment;
    juce::SliderParameterAttachment ratio2SliderAttachment;
    juce::SliderParameterAttachment ratio3SliderAttachment;
    juce::SliderParameterAttachment ratio4SliderAttachment;

    juce::Label envelopeLabel { "envelope label", "Envelope" };
    juce::Slider attackSlider { "attack slider" };
    juce::Slider decaySlider { "decay slider" };
    juce::Slider sustainSlider { "sustain slider" };
    juce::Slider releaseSlider { "release slider" };
    juce::SliderParameterAttachment attackSliderAttachment;
    juce::SliderParameterAttachment decaySliderAttachment;
    juce::SliderParameterAttachment sustainSliderAttachment;
    juce::SliderParameterAttachment releaseSliderAttachment;

    juce::Label noiseLabel { "noise label", "Noise" };
    juce::Slider noiseSlider { "noise slider" };
    juce::SliderParameterAttachment noiseSliderAttachment;

    juce::WebSliderRelay webGain1Relay;
    juce::WebSliderRelay webGain2Relay;
    juce::WebSliderRelay webGain3Relay;
    juce::WebSliderRelay webGain4Relay;
    juce::WebSliderRelay webRatio1Relay;
    juce::WebSliderRelay webRatio2Relay;
    juce::WebSliderRelay webRatio3Relay;
    juce::WebSliderRelay webRatio4Relay;

    juce::WebSliderRelay webAttackRelay;
    juce::WebSliderRelay webDecayRelay;
    juce::WebSliderRelay webSustainRelay;
    juce::WebSliderRelay webReleaseRelay;
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

    juce::WebSliderParameterAttachment webAttackSliderAttachment;
    juce::WebSliderParameterAttachment webDecaySliderAttachment;
    juce::WebSliderParameterAttachment webSustainSliderAttachment;
    juce::WebSliderParameterAttachment webReleaseSliderAttachment;
    juce::WebSliderParameterAttachment webNoiseSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};