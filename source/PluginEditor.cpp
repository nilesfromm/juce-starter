#include "PluginEditor.h"

#include "PluginProcessor.h"
#include "include/parameters/Parameters.h"
#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include <WebViewFiles.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>
#include <optional>
#include <ranges>

namespace
{
    std::vector<std::byte> streamToVector (juce::InputStream& stream)
    {
        using namespace juce;
        const auto sizeInBytes = static_cast<size_t> (stream.getTotalLength());
        std::vector<std::byte> result (sizeInBytes);
        stream.setPosition (0);
        [[maybe_unused]] const auto bytesRead =
            stream.read (result.data(), result.size());
        jassert (bytesRead == static_cast<ssize_t> (sizeInBytes));
        return result;
    }

    static const char* getMimeForExtension (const juce::String& extension)
    {
        static const std::unordered_map<juce::String, const char*> mimeMap = {
            { { "htm" }, "text/html" },
            { { "html" }, "text/html" },
            { { "txt" }, "text/plain" },
            { { "jpg" }, "image/jpeg" },
            { { "jpeg" }, "image/jpeg" },
            { { "svg" }, "image/svg+xml" },
            { { "ico" }, "image/vnd.microsoft.icon" },
            { { "json" }, "application/json" },
            { { "png" }, "image/png" },
            { { "css" }, "text/css" },
            { { "map" }, "application/json" },
            { { "js" }, "text/javascript" },
            { { "woff2" }, "font/woff2" }
        };

        if (const auto it = mimeMap.find (extension.toLowerCase());
            it != mimeMap.end())
            return it->second;

        jassertfalse;
        return "";
    }

    juce::Identifier getExampleEventId()
    {
        static const juce::Identifier id { "exampleEvent" };
        return id;
    }

#ifndef ZIPPED_FILES_PREFIX
    #error \
        "You must provide the prefix of zipped web UI files' paths, e.g., 'public/', in the ZIPPED_FILES_PREFIX compile definition"
#endif

    /**
 * @brief Get a web UI file as bytes
 *
 * @param filepath path of the form "index.html", "js/index.js", etc.
 * @return std::vector<std::byte> with bytes of a read file or an empty vector
 * if the file is not contained in webview_files.zip
 */
    std::vector<std::byte> getWebViewFileAsBytes (const juce::String& filepath)
    {
        juce::MemoryInputStream zipStream { webview_files::webview_files_zip,
            webview_files::webview_files_zipSize,
            false };
        juce::ZipFile zipFile { zipStream };

        if (auto* zipEntry = zipFile.getEntry (ZIPPED_FILES_PREFIX + filepath))
        {
            const std::unique_ptr<juce::InputStream> entryStream {
                zipFile.createStreamForEntry (*zipEntry)
            };

            if (entryStream == nullptr)
            {
                jassertfalse;
                return {};
            }

            return streamToVector (*entryStream);
        }

        return {};
    }

    constexpr auto LOCAL_DEV_SERVER_ADDRESS = "http://localhost:3000/";
} // namespace

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p),
      gain1SliderAttachment {
          *processorRef.getState().getParameter (id::gain1.getParamID()),
          gain1Slider,
          nullptr
      },
      gain2SliderAttachment {
          *processorRef.getState().getParameter (id::gain2.getParamID()),
          gain2Slider,
          nullptr
      },
      gain3SliderAttachment {
          *processorRef.getState().getParameter (id::gain3.getParamID()),
          gain3Slider,
          nullptr
      },
      gain4SliderAttachment {
          *processorRef.getState().getParameter (id::gain4.getParamID()),
          gain4Slider,
          nullptr
      },
      ratio1SliderAttachment {
          *processorRef.getState().getParameter (id::ratio1.getParamID()),
          ratio1Slider,
          nullptr
      },
      ratio2SliderAttachment {
          *processorRef.getState().getParameter (id::ratio2.getParamID()),
          ratio2Slider,
          nullptr
      },
      ratio3SliderAttachment {
          *processorRef.getState().getParameter (id::ratio3.getParamID()),
          ratio3Slider,
          nullptr
      },
      ratio4SliderAttachment {
          *processorRef.getState().getParameter (id::ratio4.getParamID()),
          ratio4Slider,
          nullptr
      },

      webGain1Relay { id::gain1.getParamID() },
      webGain2Relay { id::gain2.getParamID() },
      webGain3Relay { id::gain3.getParamID() },
      webGain4Relay { id::gain4.getParamID() },
      webRatio1Relay { id::ratio1.getParamID() },
      webRatio2Relay { id::ratio2.getParamID() },
      webRatio3Relay { id::ratio3.getParamID() },
      webRatio4Relay { id::ratio4.getParamID() },

      webView {
          juce::WebBrowserComponent::Options {}
              .withBackend (
                  juce::WebBrowserComponent::Options::Backend::webview2)
              .withWinWebView2Options (
                  juce::WebBrowserComponent::Options::WinWebView2 {}
                      .withBackgroundColour (juce::Colours::white)
                      // this may be necessary for some DAWs; include for safety
                      .withUserDataFolder (juce::File::getSpecialLocation (
                          juce::File::SpecialLocationType::tempDirectory)))
              .withNativeIntegrationEnabled()
              .withResourceProvider (
                  [this] (const auto& url) { return getResource (url); },
                  // allowedOriginIn parameter is necessary to
                  // retrieve resources from the C++ backend even if
                  // on live server
                  juce::URL { LOCAL_DEV_SERVER_ADDRESS }.getOrigin())
              .withInitialisationData ("vendor", JUCE_COMPANY_NAME)
              .withInitialisationData ("pluginName", JUCE_PRODUCT_NAME)
              .withInitialisationData ("pluginVersion", JUCE_PRODUCT_VERSION)
              .withUserScript ("console.log(\"C++ backend here: This is run "
                               "before any other loading happens\");")
              .withEventListener (
                  "exampleJavaScriptEvent",
                  [this] (juce::var objectFromFrontend) {
                      labelUpdatedFromJavaScript.setText (
                          "example JavaScript event occurred with value " + objectFromFrontend.getProperty ("emittedCount", 0).toString(),
                          juce::dontSendNotification);
                  })
              .withNativeFunction (
                  juce::Identifier { "nativeFunction" },
                  [this] (const juce::Array<juce::var>& args,
                      juce::WebBrowserComponent::NativeFunctionCompletion
                          completion) {
                      nativeFunction (args, std::move (completion));
                  })
              .withOptionsFrom (webGain1Relay)
              .withOptionsFrom (webGain2Relay)
              .withOptionsFrom (webGain3Relay)
              .withOptionsFrom (webGain4Relay)
              .withOptionsFrom (webRatio1Relay)
              .withOptionsFrom (webRatio2Relay)
              .withOptionsFrom (webRatio3Relay)
              .withOptionsFrom (webRatio4Relay)
      },
      webGain1SliderAttachment {
          *processorRef.getState().getParameter (id::gain1.getParamID()),
          webGain1Relay,
          nullptr
      },
      webGain2SliderAttachment {
          *processorRef.getState().getParameter (id::gain2.getParamID()),
          webGain2Relay,
          nullptr
      },
      webGain3SliderAttachment {
          *processorRef.getState().getParameter (id::gain3.getParamID()),
          webGain3Relay,
          nullptr
      },
      webGain4SliderAttachment {
          *processorRef.getState().getParameter (id::gain4.getParamID()),
          webGain4Relay,
          nullptr
      },
      webRatio1SliderAttachment {
          *processorRef.getState().getParameter (id::ratio1.getParamID()),
          webRatio1Relay,
          nullptr
      },
      webRatio2SliderAttachment {
          *processorRef.getState().getParameter (id::ratio2.getParamID()),
          webRatio2Relay,
          nullptr
      },
      webRatio3SliderAttachment {
          *processorRef.getState().getParameter (id::ratio3.getParamID()),
          webRatio3Relay,
          nullptr
      },
      webRatio4SliderAttachment {
          *processorRef.getState().getParameter (id::ratio4.getParamID()),
          webRatio4Relay,
          nullptr
      }
{
    // juce::ignoreUnused (processorRef);

    addAndMakeVisible (webView);

    // WebBrowserComponent can display any URL
    // webView.goToURL("https://juce.com");

    // This is necessary if we want to use a ResourceProvider
    // webView.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // This can be used for hot reloading
    webView.goToURL (LOCAL_DEV_SERVER_ADDRESS);

    runJavaScriptButton.onClick = [this] {
        constexpr auto JAVASCRIPT_TO_RUN { "console.log(\"Hello from C++!\");" };
        webView.evaluateJavascript (
            JAVASCRIPT_TO_RUN,
            [] (juce::WebBrowserComponent::EvaluationResult result) {
                if (const auto* resultPtr = result.getResult())
                {
                    std::cout << "JavaScript evaluation result: "
                              << resultPtr->toString() << std::endl;
                }
                else
                {
                    std::cout << "JavaScript evaluation failed because "
                              << result.getError()->message << std::endl;
                }
            });
    };
    addAndMakeVisible (runJavaScriptButton);

    emitJavaScriptEventButton.onClick = [this] {
        static const juce::var valueToEmit { 42.0 };
        webView.emitEventIfBrowserIsVisible (getExampleEventId(), valueToEmit);
    };
    addAndMakeVisible (emitJavaScriptEventButton);

    addAndMakeVisible (labelUpdatedFromJavaScript);

    addAndMakeVisible (gainLabel);
    gain1Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    gain2Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    gain3Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    gain4Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    addAndMakeVisible (gain1Slider);
    addAndMakeVisible (gain2Slider);
    addAndMakeVisible (gain3Slider);
    addAndMakeVisible (gain4Slider);

    addAndMakeVisible (ratioLabel);
    ratio1Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    ratio2Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    ratio3Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    ratio4Slider.setSliderStyle (juce::Slider::SliderStyle::LinearBar);
    addAndMakeVisible (ratio1Slider);
    addAndMakeVisible (ratio2Slider);
    addAndMakeVisible (ratio3Slider);
    addAndMakeVisible (ratio4Slider);

    setResizable (true, true);
    setSize (800, 800);

    startTimer (60);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::resized()
{
    auto bounds = getBounds();
    webView.setBounds (bounds.removeFromRight (getWidth() / 2));
    runJavaScriptButton.setBounds (bounds.removeFromTop (50).reduced (5));
    emitJavaScriptEventButton.setBounds (bounds.removeFromTop (50).reduced (5));
    labelUpdatedFromJavaScript.setBounds (bounds.removeFromTop (50).reduced (5));
    gainLabel.setBounds (bounds.removeFromTop (50).reduced (5));
    gain1Slider.setBounds (bounds.removeFromTop (50).reduced (5));
    gain2Slider.setBounds (bounds.removeFromTop (50).reduced (5));
    gain3Slider.setBounds (bounds.removeFromTop (50).reduced (5));
    gain4Slider.setBounds (bounds.removeFromTop (50).reduced (5));
    ratioLabel.setBounds (bounds.removeFromTop (50).reduced (5));
    ratio1Slider.setBounds (bounds.removeFromTop (50).reduced (5));
    ratio2Slider.setBounds (bounds.removeFromTop (50).reduced (5));
    ratio3Slider.setBounds (bounds.removeFromTop (50).reduced (5));
    ratio4Slider.setBounds (bounds.removeFromTop (50).reduced (5));
}

void PluginEditor::timerCallback()
{
    webView.emitEventIfBrowserIsVisible ("outputLevel", juce::var {});
}

auto PluginEditor::getResource (const juce::String& url) const
    -> std::optional<Resource>
{
    std::cout << "ResourceProvider called with " << url << std::endl;

    const auto resourceToRetrieve =
        url == "/" ? "index.html" : url.fromFirstOccurrenceOf ("/", false, false);

    if (resourceToRetrieve == "outputLevel.json")
    {
        juce::DynamicObject::Ptr levelData { new juce::DynamicObject {} };
        levelData->setProperty ("left", processorRef.outputLevelLeft.load());
        const auto jsonString = juce::JSON::toString (levelData.get());
        juce::MemoryInputStream stream { jsonString.getCharPointer(),
            jsonString.getNumBytesAsUTF8(),
            false };
        return juce::WebBrowserComponent::Resource {
            streamToVector (stream), juce::String { "application/json" }
        };
    }

    const auto resource = getWebViewFileAsBytes (resourceToRetrieve);
    if (!resource.empty())
    {
        const auto extension =
            resourceToRetrieve.fromLastOccurrenceOf (".", false, false);
        return Resource { std::move (resource), getMimeForExtension (extension) };
    }

    return std::nullopt;
}

void PluginEditor::nativeFunction (
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion)
{
    using namespace std::views;
    juce::String concatenatedString;
    for (const auto& string : args | transform (&juce::var::toString))
    {
        concatenatedString += string;
    }
    labelUpdatedFromJavaScript.setText (
        "Native function called with args: " + concatenatedString,
        juce::dontSendNotification);
    completion ("nativeFunction callback: All OK!");
}
