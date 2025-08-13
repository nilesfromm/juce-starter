import React, { useRef } from "react";
import "./App.css";
import * as Juce from "./juce/index.js";

function App() {
  const sliderRef = useRef<HTMLInputElement>(null);
  console.log("JUCE frontend library successfully imported");
  console.log(Juce);
  const nativeFunction = Juce.getNativeFunction("nativeFunction") as (
    a: string,
    b: number,
    c: null
  ) => Promise<number>;

  const windowJuce = (window as any).__JUCE__;
  const data = windowJuce.initialisationData;

  let emittedCount = 0;

  const handleNativeFunctionButtonClick = () => {
    nativeFunction("one", 2, null).then((result) => {
      console.log(result);
    });
  };

  const handleButtonClick = () => {
    emittedCount++;
    windowJuce.backend.emitEvent("exampleJavaScriptEvent", {
      emittedCount: emittedCount,
    });
  };

  const sliderState = Juce.getSliderState("GAIN");
  const handleSliderChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    sliderState.setNormalisedValue(event.target.value);
  };

  sliderState.valueChangedEvent.addListener(() => {
    if (sliderRef.current) {
      sliderRef.current.value = sliderState.getNormalisedValue();
    }
  });

  return (
    <div className="size-full h-screen flex flex-col gap-2 items-center justify-center bg-black text-white">
      <p className="text-2xl">hello juce</p>
      <br />
      <p>{data.vendor ?? "no vendor"}</p>
      <p>{data.pluginName ?? "no plugin name"}</p>
      <p>{data.pluginVersion ?? "no plugin version"}</p>
      <br />
      <button
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
      </button>
      <div className="w-full p-4 bg-blue-500/50 flex items-center justify-center">
        <input
          ref={sliderRef}
          type="range"
          id="gainSlider"
          min="0"
          max="1"
          step="0.001"
          className="w-full"
          onChange={handleSliderChange}
        />
      </div>
    </div>
  );
}

export default App;
