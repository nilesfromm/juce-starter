import { useEffect, useRef, useState } from "react";
import * as Juce from "../juce/index.js";

interface NumberInputProps {
  label: string;
  id: string;
  min: number;
  max: number;
  step: number;
  onChange?: (value: number) => void;
}

const NumberInput = ({
  label,
  id,
  min,
  max,
  step,
  onChange,
}: NumberInputProps) => {
  const currentValueRef = useRef(0);
  const isDragging = useRef(false);

  const sliderRef = useRef<HTMLInputElement>(null);
  const sliderState = Juce.getSliderState(id);

  const [sliderValue, setSliderValue] = useState<number>(
    sliderState.getNormalisedValue()
  );

  const handleChange = (value: number) => {
    const constrainedValue = Math.max(min, Math.min(max, value));
    const normalisedValue = constrainedValue / max;
    console.log("constrainedValue: ", constrainedValue);
    console.log("normalisedValue: ", normalisedValue);
    sliderState.setNormalisedValue(normalisedValue);
    if (onChange) {
      onChange(normalisedValue);
    }
  };

  useEffect(() => {
    const updateSlider = () => {
      if (sliderRef.current) {
        const updatedValue = sliderState.getNormalisedValue();
        const normalisedValue = updatedValue * max;
        const roundedValue = Math.round(normalisedValue * 10000) / 10000;
        sliderRef.current.value = roundedValue.toString();
        setSliderValue(roundedValue);
        if (onChange) {
          onChange(roundedValue);
        }
      }
    };
    updateSlider();
    sliderState.valueChangedEvent.addListener(updateSlider);
    return sliderState.valueChangedEvent.removeListener(updateSlider);
  }, [onChange, sliderState]);

  const handleMouseDown = (e: React.MouseEvent<HTMLButtonElement>) => {
    isDragging.current = true;
    currentValueRef.current = sliderState.getNormalisedValue();
    document.addEventListener("mousemove", handleMouseMove);
    document.addEventListener("mouseup", handleMouseUp);
  };

  const handleMouseUp = (e: MouseEvent) => {
    e.preventDefault();
    isDragging.current = false;
    document.removeEventListener("mousemove", handleMouseMove);
    document.removeEventListener("mouseup", handleMouseUp);
    // Ensure we update with the final value
    handleChange(Math.max(min, Math.min(max, currentValueRef.current)));
  };

  const handleMouseMove = (e: MouseEvent) => {
    e.preventDefault();
    if (!isDragging) return;

    const sensitivity = step;
    const delta = -e.movementY * sensitivity;
    currentValueRef.current = Number(
      Math.max(min, Math.min(max, currentValueRef.current + delta)).toFixed(2)
    );
    handleChange(currentValueRef.current);
  };

  return (
    <div className="flex text-[12px]">
      <button
        className="size-6 bg-[#333333] flex items-center justify-center"
        onMouseDown={handleMouseDown}
      >
        <span className="text-white">{label}</span>
      </button>
      <input
        ref={sliderRef}
        type="number"
        className="bg-[#D9D9D9] h-6 w-full text-center"
        value={isDragging.current ? currentValueRef.current : sliderValue}
        onChange={(e) => setSliderValue(Number(e.target.value))}
        onBlur={() => handleChange(sliderValue)}
        onKeyDown={(e) => {
          if (e.key === "Enter") {
            handleChange(sliderValue);
          }
        }}
      />
    </div>
  );
};

export default NumberInput;
