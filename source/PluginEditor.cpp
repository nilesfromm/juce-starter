#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "parameters/ParameterSchema.h"
#include <WebViewFiles.h>
#include <optional>

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

#ifndef ZIPPED_FILES_PREFIX
    #error \
        "You must provide the prefix of zipped web UI files' paths, e.g., 'public/', in the ZIPPED_FILES_PREFIX compile definition"
#endif

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
      webView {
          juce::WebBrowserComponent::Options {}
              .withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
              .withWinWebView2Options (
                  juce::WebBrowserComponent::Options::WinWebView2 {}
                      .withBackgroundColour (juce::Colours::white)
                      .withUserDataFolder (juce::File::getSpecialLocation (
                          juce::File::SpecialLocationType::tempDirectory)))
              .withNativeIntegrationEnabled()
              .withResourceProvider (
                  [this] (const auto& url) { return getResource (url); },
                  juce::URL { LOCAL_DEV_SERVER_ADDRESS }.getOrigin())
              .withInitialisationData ("vendor", JUCE_COMPANY_NAME)
              .withInitialisationData ("pluginName", JUCE_PRODUCT_NAME)
              .withInitialisationData ("pluginVersion", JUCE_PRODUCT_VERSION)
              .withNativeFunction (juce::Identifier { "updateParam" },
                  [this] (const juce::Array<juce::var>& args,
                      juce::WebBrowserComponent::NativeFunctionCompletion completion) {
                      handleParamUpdate (args, std::move (completion));
                  })
              .withNativeFunction (juce::Identifier { "loadState" },
                  [this] (const juce::Array<juce::var>& args,
                      juce::WebBrowserComponent::NativeFunctionCompletion completion) {
                      handleLoadState (args, std::move (completion));
                  })
      }
{
    addAndMakeVisible (webView);

    // This can be used for hot reloading
    webView.goToURL (LOCAL_DEV_SERVER_ADDRESS);

    // This is necessary if we want to use a ResourceProvider
    // webView.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

#ifndef JUCE_DEBUG
    // Disable right-click context menu in production builds
    webView.evaluateJavascript ("document.addEventListener('contextmenu', event => event.preventDefault());");
#endif

    // setResizable (true, true); (need to update to make UI responsive first)
    setSize (784, 530);

    // Register as listener for all parameters (for DAW automation → JS sync)
    for (const auto& param : PARAM_SCHEMA)
    {
        processorRef.getState().addParameterListener (param.id, this);
    }

    startTimer (60);
}

PluginEditor::~PluginEditor()
{
    for (const auto& param : PARAM_SCHEMA)
    {
        processorRef.getState().removeParameterListener (param.id, this);
    }
}

void PluginEditor::resized()
{
    webView.setBounds (getBounds());
}

void PluginEditor::timerCallback()
{
    webView.emitEventIfBrowserIsVisible ("outputLevel", juce::var {});
}

// JS → C++: updateParam("h1_gain", 0.75)
void PluginEditor::handleParamUpdate (const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion)
{
    if (args.size() >= 2)
    {
        const juce::String paramId = args[0].toString();
        const float value = static_cast<float> (args[1]);

        if (auto* param = dynamic_cast<juce::AudioParameterFloat*> (
                processorRef.getState().getParameter (paramId)))
        {
            param->setValueNotifyingHost (
                param->getNormalisableRange().convertTo0to1 (value));
        }
    }

    completion (juce::var {});
}

// JS → C++: loadState() — returns { schema: [...], values: { "h1_gain": 0.75, ... } }
void PluginEditor::handleLoadState (const juce::Array<juce::var>& /*args*/,
    juce::WebBrowserComponent::NativeFunctionCompletion completion)
{
    auto* result = new juce::DynamicObject();

    // Build schema array
    juce::Array<juce::var> schemaArray;
    for (const auto& p : PARAM_SCHEMA)
    {
        auto* entry = new juce::DynamicObject();
        entry->setProperty ("id", juce::String (p.id));
        entry->setProperty ("name", juce::String (p.name));
        entry->setProperty ("min", p.min);
        entry->setProperty ("max", p.max);
        entry->setProperty ("default", p.defaultVal);
        entry->setProperty ("step", p.step);
        entry->setProperty ("skew", p.skew);
        schemaArray.add (juce::var (entry));
    }
    result->setProperty ("schema", schemaArray);

    // Build current values
    auto* values = new juce::DynamicObject();
    for (const auto& p : PARAM_SCHEMA)
    {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*> (
                processorRef.getState().getParameter (p.id)))
        {
            values->setProperty (p.id, static_cast<double> (param->get()));
        }
    }
    result->setProperty ("values", juce::var (values));

    completion (juce::var (result));
}

// C++ → JS: DAW automation / preset change pushes to webview
void PluginEditor::parameterChanged (const juce::String& parameterID, float newValue)
{
    // parameterChanged can be called from the audio thread, so we need to
    // bounce to the message thread for the webview call
    juce::MessageManager::callAsync ([this, parameterID, newValue]() {
        auto* obj = new juce::DynamicObject();
        obj->setProperty ("id", parameterID);
        obj->setProperty ("value", static_cast<double> (newValue));
        webView.emitEventIfBrowserIsVisible (
            juce::Identifier { "paramChanged" }, juce::var (obj));
    });
}

auto PluginEditor::getResource (const juce::String& url) const
    -> std::optional<Resource>
{
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
