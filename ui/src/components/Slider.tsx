import React, { useRef, useEffect } from "react";
import * as Juce from "../juce/index.js";

interface SliderProps
{
  sliderId: string
}

const Slider = ({sliderId}: SliderProps) => {
  const sliderRef = useRef<HTMLInputElement>(null);
  const sliderState = Juce.getSliderState(sliderId);

  const handleSliderChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    sliderState.setNormalisedValue(event.target.value);
  };

  useEffect(()=>{
    const updateSlider = () => {
      if (sliderRef.current) {
        sliderRef.current.value = sliderState.getNormalisedValue();
      }
    }
    updateSlider();
    sliderState.valueChangedEvent.addListener(updateSlider);
    return(
      sliderState.valueChangedEvent.removeListener(updateSlider)
    )
  },[])
  

  return (
    <div className="w-full p-4 bg-blue-300/50 flex items-center justify-center">
      <input
        ref={sliderRef}
        type="range"
        id={`${sliderId}-slider`}
        min="0"
        max="1"
        step="0.001"
        className="w-full"
        onChange={handleSliderChange}
      />
    </div>
  );
}
 
export default Slider;