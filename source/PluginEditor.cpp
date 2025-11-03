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
      webRatioRelays { { juce::WebSliderRelay { id::h1_ratio.getParamID() },
          juce::WebSliderRelay { id::h2_ratio.getParamID() },
          juce::WebSliderRelay { id::h3_ratio.getParamID() },
          juce::WebSliderRelay { id::h4_ratio.getParamID() },
          juce::WebSliderRelay { id::h5_ratio.getParamID() },
          juce::WebSliderRelay { id::h6_ratio.getParamID() },
          juce::WebSliderRelay { id::h7_ratio.getParamID() },
          juce::WebSliderRelay { id::h8_ratio.getParamID() },
          juce::WebSliderRelay { id::h9_ratio.getParamID() } } },
      webGainRelays { { juce::WebSliderRelay { id::h1_gain.getParamID() },
          juce::WebSliderRelay { id::h2_gain.getParamID() },
          juce::WebSliderRelay { id::h3_gain.getParamID() },
          juce::WebSliderRelay { id::h4_gain.getParamID() },
          juce::WebSliderRelay { id::h5_gain.getParamID() },
          juce::WebSliderRelay { id::h6_gain.getParamID() },
          juce::WebSliderRelay { id::h7_gain.getParamID() },
          juce::WebSliderRelay { id::h8_gain.getParamID() },
          juce::WebSliderRelay { id::h9_gain.getParamID() } } },
      webAttackRelays { { juce::WebSliderRelay { id::h1_attack.getParamID() },
          juce::WebSliderRelay { id::h2_attack.getParamID() },
          juce::WebSliderRelay { id::h3_attack.getParamID() },
          juce::WebSliderRelay { id::h4_attack.getParamID() },
          juce::WebSliderRelay { id::h5_attack.getParamID() },
          juce::WebSliderRelay { id::h6_attack.getParamID() },
          juce::WebSliderRelay { id::h7_attack.getParamID() },
          juce::WebSliderRelay { id::h8_attack.getParamID() },
          juce::WebSliderRelay { id::h9_attack.getParamID() } } },
      webDecayRelays { { juce::WebSliderRelay { id::h1_decay.getParamID() },
          juce::WebSliderRelay { id::h2_decay.getParamID() },
          juce::WebSliderRelay { id::h3_decay.getParamID() },
          juce::WebSliderRelay { id::h4_decay.getParamID() },
          juce::WebSliderRelay { id::h5_decay.getParamID() },
          juce::WebSliderRelay { id::h6_decay.getParamID() },
          juce::WebSliderRelay { id::h7_decay.getParamID() },
          juce::WebSliderRelay { id::h8_decay.getParamID() },
          juce::WebSliderRelay { id::h9_decay.getParamID() } } },
      webSustainRelays { { juce::WebSliderRelay { id::h1_sustain.getParamID() },
          juce::WebSliderRelay { id::h2_sustain.getParamID() },
          juce::WebSliderRelay { id::h3_sustain.getParamID() },
          juce::WebSliderRelay { id::h4_sustain.getParamID() },
          juce::WebSliderRelay { id::h5_sustain.getParamID() },
          juce::WebSliderRelay { id::h6_sustain.getParamID() },
          juce::WebSliderRelay { id::h7_sustain.getParamID() },
          juce::WebSliderRelay { id::h8_sustain.getParamID() },
          juce::WebSliderRelay { id::h9_sustain.getParamID() } } },
      webReleaseRelays { { juce::WebSliderRelay { id::h1_release.getParamID() },
          juce::WebSliderRelay { id::h2_release.getParamID() },
          juce::WebSliderRelay { id::h3_release.getParamID() },
          juce::WebSliderRelay { id::h4_release.getParamID() },
          juce::WebSliderRelay { id::h5_release.getParamID() },
          juce::WebSliderRelay { id::h6_release.getParamID() },
          juce::WebSliderRelay { id::h7_release.getParamID() },
          juce::WebSliderRelay { id::h8_release.getParamID() },
          juce::WebSliderRelay { id::h9_release.getParamID() } } },
      webNoiseRelay { id::noise.getParamID() },

      webView {
          [&] {
              auto options = juce::WebBrowserComponent::Options {}
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
                                 .withInitialisationData ("pluginVersion", JUCE_PRODUCT_VERSION);

              // Add all harmonic parameter relays
              for (int i = 0; i < NUM_HARMONICS; ++i)
              {
                  options = options
                                .withOptionsFrom (webGainRelays[i])
                                .withOptionsFrom (webRatioRelays[i])
                                .withOptionsFrom (webAttackRelays[i])
                                .withOptionsFrom (webDecayRelays[i])
                                .withOptionsFrom (webSustainRelays[i])
                                .withOptionsFrom (webReleaseRelays[i]);
              }

              return options.withOptionsFrom (webNoiseRelay);
          }()
      },

      webGainAttachments {
          { juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h1_gain.getParamID()), webGainRelays[0], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h2_gain.getParamID()), webGainRelays[1], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h3_gain.getParamID()), webGainRelays[2], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h4_gain.getParamID()), webGainRelays[3], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h5_gain.getParamID()), webGainRelays[4], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h6_gain.getParamID()), webGainRelays[5], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h7_gain.getParamID()), webGainRelays[6], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h8_gain.getParamID()), webGainRelays[7], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h9_gain.getParamID()), webGainRelays[8], nullptr } }
      },
      webRatioAttachments {
          { juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h1_ratio.getParamID()), webRatioRelays[0], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h2_ratio.getParamID()), webRatioRelays[1], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h3_ratio.getParamID()), webRatioRelays[2], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h4_ratio.getParamID()), webRatioRelays[3], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h5_ratio.getParamID()), webRatioRelays[4], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h6_ratio.getParamID()), webRatioRelays[5], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h7_ratio.getParamID()), webRatioRelays[6], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h8_ratio.getParamID()), webRatioRelays[7], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h9_ratio.getParamID()), webRatioRelays[8], nullptr } }
      },
      webAttackAttachments {
          { juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h1_attack.getParamID()), webAttackRelays[0], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h2_attack.getParamID()), webAttackRelays[1], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h3_attack.getParamID()), webAttackRelays[2], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h4_attack.getParamID()), webAttackRelays[3], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h5_attack.getParamID()), webAttackRelays[4], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h6_attack.getParamID()), webAttackRelays[5], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h7_attack.getParamID()), webAttackRelays[6], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h8_attack.getParamID()), webAttackRelays[7], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h9_attack.getParamID()), webAttackRelays[8], nullptr } }
      },
      webDecayAttachments {
          { juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h1_decay.getParamID()), webDecayRelays[0], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h2_decay.getParamID()), webDecayRelays[1], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h3_decay.getParamID()), webDecayRelays[2], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h4_decay.getParamID()), webDecayRelays[3], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h5_decay.getParamID()), webDecayRelays[4], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h6_decay.getParamID()), webDecayRelays[5], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h7_decay.getParamID()), webDecayRelays[6], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h8_decay.getParamID()), webDecayRelays[7], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h9_decay.getParamID()), webDecayRelays[8], nullptr } }
      },
      webSustainAttachments {
          { juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h1_sustain.getParamID()), webSustainRelays[0], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h2_sustain.getParamID()), webSustainRelays[1], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h3_sustain.getParamID()), webSustainRelays[2], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h4_sustain.getParamID()), webSustainRelays[3], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h5_sustain.getParamID()), webSustainRelays[4], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h6_sustain.getParamID()), webSustainRelays[5], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h7_sustain.getParamID()), webSustainRelays[6], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h8_sustain.getParamID()), webSustainRelays[7], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h9_sustain.getParamID()), webSustainRelays[8], nullptr } }
      },
      webReleaseAttachments {
          { juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h1_release.getParamID()), webReleaseRelays[0], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h2_release.getParamID()), webReleaseRelays[1], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h3_release.getParamID()), webReleaseRelays[2], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h4_release.getParamID()), webReleaseRelays[3], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h5_release.getParamID()), webReleaseRelays[4], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h6_release.getParamID()), webReleaseRelays[5], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h7_release.getParamID()), webReleaseRelays[6], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h8_release.getParamID()), webReleaseRelays[7], nullptr },
              juce::WebSliderParameterAttachment { *processorRef.getState().getParameter (id::h9_release.getParamID()), webReleaseRelays[8], nullptr } }
      },
      webNoiseAttachment {
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
    // webView.goToURL (juce::WebBrowserComponent::getResourceProviderRoot());

    // This can be used for hot reloading
    webView.goToURL (LOCAL_DEV_SERVER_ADDRESS);

#ifndef JUCE_DEBUG
    // Disable right-click context menu in production builds
    webView.evaluateJavascript ("document.addEventListener('contextmenu', event => event.preventDefault());");
#endif

    // setResizable (true, true);
    setSize (784, 530);

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
