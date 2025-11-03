import "./App.css";
// import Slider from "./components/Slider";
import Voice from "./components/Voice";
import * as Juce from "./juce/index.js";

function App() {
  console.log("JUCE frontend library successfully imported");
  console.log(Juce);
  // const nativeFunction = Juce.getNativeFunction("nativeFunction") as (
  //   a: string,
  //   b: number,
  //   c: null
  // ) => Promise<number>;

  // const windowJuce = (window as any).__JUCE__;
  // // const data = windowJuce.initialisationData;

  // let emittedCount = 0;

  // const handleNativeFunctionButtonClick = () => {
  //   nativeFunction("one", 2, null).then((result) => {
  //     console.log(result);
  //   });
  // };

  // const handleButtonClick = () => {
  //   emittedCount++;
  //   windowJuce.backend.emitEvent("exampleJavaScriptEvent", {
  //     emittedCount: emittedCount,
  //   });
  // };

  return (
    <div className="grid grid-cols-3 gap-4 items-center justify-center bg-[#F2F2F2] text-[#333333] p-4">
      {/* <button
        className="border border-white rounded-lg px-2 py-1 hover:cursor-pointer hover:bg-white/20 transition-colors duration-200"
        onClick={handleButtonClick}
      >
        Emit Event
      </button>
      <button
        className="border border-white rounded-lg px-2 py-1 hover:cursor-pointer hover:bg-white/20 transition-colors duration-200"
        onClick={handleNativeFunctionButtonClick}
      >
        Native Function
      </button> */}
      <Voice voiceId="h1" voiceName="H1" />
      <Voice voiceId="h2" voiceName="H2" />
      <Voice voiceId="h3" voiceName="H3" />
      <Voice voiceId="h4" voiceName="H4" />
      <Voice voiceId="h5" voiceName="H5" />
      <Voice voiceId="h6" voiceName="H6" />
      <Voice voiceId="h7" voiceName="H7" />
      <Voice voiceId="h8" voiceName="H8" />
      <Voice voiceId="h9" voiceName="N1" />
      {/* <Slider sliderId="attack" />
      <Slider sliderId="decay" />
      <Slider sliderId="sustain" />
      <Slider sliderId="release" /> */}
      {/* <Slider sliderId="noise" /> */}
    </div>
  );
}

export default App;
