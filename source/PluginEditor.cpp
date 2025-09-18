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
      webRatio1Relay { id::ratio1.getParamID() },
      webRatio2Relay { id::ratio2.getParamID() },
      webRatio3Relay { id::ratio3.getParamID() },
      webRatio4Relay { id::ratio4.getParamID() },

      webGain1Relay { id::h1_gain.getParamID() },
      webA1Relay { id::h1_attack.getParamID() },
      webD1Relay { id::h1_decay.getParamID() },
      webS1Relay { id::h1_sustain.getParamID() },
      webR1Relay { id::h1_release.getParamID() },

      webGain2Relay { id::h2_gain.getParamID() },
      webA2Relay { id::h2_attack.getParamID() },
      webD2Relay { id::h2_decay.getParamID() },
      webS2Relay { id::h2_sustain.getParamID() },
      webR2Relay { id::h2_release.getParamID() },

      webGain3Relay { id::h3_gain.getParamID() },
      webA3Relay { id::h3_attack.getParamID() },
      webD3Relay { id::h3_decay.getParamID() },
      webS3Relay { id::h3_sustain.getParamID() },
      webR3Relay { id::h3_release.getParamID() },

      webGain4Relay { id::h4_gain.getParamID() },
      webA4Relay { id::h4_attack.getParamID() },
      webD4Relay { id::h4_decay.getParamID() },
      webS4Relay { id::h4_sustain.getParamID() },
      webR4Relay { id::h4_release.getParamID() },

      webNoiseRelay { id::noise.getParamID() },

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

              .withOptionsFrom (webGain1Relay)
              .withOptionsFrom (webGain2Relay)
              .withOptionsFrom (webGain3Relay)
              .withOptionsFrom (webGain4Relay)
              .withOptionsFrom (webRatio1Relay)
              .withOptionsFrom (webRatio2Relay)
              .withOptionsFrom (webRatio3Relay)
              .withOptionsFrom (webRatio4Relay)
              .withOptionsFrom (webA1Relay)
              .withOptionsFrom (webD1Relay)
              .withOptionsFrom (webS1Relay)
              .withOptionsFrom (webR1Relay)
              .withOptionsFrom (webA2Relay)
              .withOptionsFrom (webD2Relay)
              .withOptionsFrom (webS2Relay)
              .withOptionsFrom (webR2Relay)
              .withOptionsFrom (webA3Relay)
              .withOptionsFrom (webD3Relay)
              .withOptionsFrom (webS3Relay)
              .withOptionsFrom (webR3Relay)
              .withOptionsFrom (webA4Relay)
              .withOptionsFrom (webD4Relay)
              .withOptionsFrom (webS4Relay)
              .withOptionsFrom (webR4Relay)
              .withOptionsFrom (webNoiseRelay)
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
      },
      webGain1Attachment {
          *processorRef.getState().getParameter (id::h1_gain.getParamID()),
          webGain1Relay,
          nullptr
      },
      webA1Attachment {
          *processorRef.getState().getParameter (id::h1_attack.getParamID()),
          webA1Relay,
          nullptr
      },
      webD1Attachment {
          *processorRef.getState().getParameter (id::h1_decay.getParamID()),
          webD1Relay,
          nullptr
      },
      webS1Attachment {
          *processorRef.getState().getParameter (id::h1_sustain.getParamID()),
          webS1Relay,
          nullptr
      },
      webR1Attachment {
          *processorRef.getState().getParameter (id::h1_release.getParamID()),
          webR1Relay,
          nullptr
      },
      webGain2Attachment {
          *processorRef.getState().getParameter (id::h2_gain.getParamID()),
          webGain2Relay,
          nullptr
      },
      webA2Attachment {
          *processorRef.getState().getParameter (id::h2_attack.getParamID()),
          webA2Relay,
          nullptr
      },
      webD2Attachment {
          *processorRef.getState().getParameter (id::h2_decay.getParamID()),
          webD2Relay,
          nullptr
      },
      webS2Attachment {
          *processorRef.getState().getParameter (id::h2_sustain.getParamID()),
          webS2Relay,
          nullptr
      },
      webR2Attachment {
          *processorRef.getState().getParameter (id::h2_release.getParamID()),
          webR2Relay,
          nullptr
      },
      webGain3Attachment {
          *processorRef.getState().getParameter (id::h3_gain.getParamID()),
          webGain3Relay,
          nullptr
      },
      webA3Attachment {
          *processorRef.getState().getParameter (id::h3_attack.getParamID()),
          webA3Relay,
          nullptr
      },
      webD3Attachment {
          *processorRef.getState().getParameter (id::h3_decay.getParamID()),
          webD3Relay,
          nullptr
      },
      webS3Attachment {
          *processorRef.getState().getParameter (id::h3_sustain.getParamID()),
          webS3Relay,
          nullptr
      },
      webR3Attachment {
          *processorRef.getState().getParameter (id::h3_release.getParamID()),
          webR3Relay,
          nullptr
      },
      webGain4Attachment {
          *processorRef.getState().getParameter (id::h4_gain.getParamID()),
          webGain4Relay,
          nullptr
      },
      webA4Attachment {
          *processorRef.getState().getParameter (id::h4_attack.getParamID()),
          webA4Relay,
          nullptr
      },
      webD4Attachment {
          *processorRef.getState().getParameter (id::h4_decay.getParamID()),
          webD4Relay,
          nullptr
      },
      webS4Attachment {
          *processorRef.getState().getParameter (id::h4_sustain.getParamID()),
          webS4Relay,
          nullptr
      },
      webR4Attachment {
          *processorRef.getState().getParameter (id::h4_release.getParamID()),
          webR4Relay,
          nullptr
      },
      webNoiseSliderAttachment {
          *processorRef.getState().getParameter (id::noise.getParamID()),
          webNoiseRelay,
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

    // setResizable (true, true);
    setSize (544, 256);

    startTimer (60);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::resized()
{
    auto bounds = getBounds();
    webView.setBounds (bounds);
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
