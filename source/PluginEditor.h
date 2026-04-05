#pragma once

#include "PluginProcessor.h"
#include "parameters/ParameterSchema.h"

#include "melatonin_inspector/melatonin_inspector.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

class PluginEditor : public juce::AudioProcessorEditor,
                     private juce::Timer,
                     private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void resized() override;
    void timerCallback() override;

private:
    using Resource = juce::WebBrowserComponent::Resource;
    std::optional<Resource> getResource (const juce::String& url) const;

    // Native function handlers
    void handleParamUpdate (const juce::Array<juce::var>& args,
        juce::WebBrowserComponent::NativeFunctionCompletion completion);
    void handleLoadState (const juce::Array<juce::var>& args,
        juce::WebBrowserComponent::NativeFunctionCompletion completion);

    // ValueTree listener — called when DAW automation or preset changes a param
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;

    juce::WebBrowserComponent webView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
