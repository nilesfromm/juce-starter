import { useState, useCallback, useRef, useEffect } from "react";
import * as Juce from "../juce/index.js";

interface HarmonicProps {
  id: string;
  ratio: number;
  gain: number;
  harmonicsWrapperBounds: DOMRect | undefined;
  onGainChange: (id: string, newGain: number) => void;
  onRatioChange: (id: string, newRatio: number) => void;
}

const Harmonic = ({
  id,
  ratio,
  gain,
  harmonicsWrapperBounds,
  onGainChange,
  onRatioChange,
}: HarmonicProps) => {
  const minHarmonic = 0;
  const maxHarmonic = 12;

  const [isDragging, setIsDragging] = useState<0 | 1 | 2>(0); // 0: not dragging, 1: dragging gain(y), 2: dragging ratio(x)
  const [isHovering, setIsHovering] = useState(false);
  const containerRef = useRef<HTMLDivElement>(null);
  const spanRef = useRef<HTMLSpanElement>(null);
  const { width: wrapperWidth, left: wrapperLeft } = harmonicsWrapperBounds || {
    width: 0,
    height: 0,
    left: 0,
  };

  const gainSliderState = Juce.getSliderState(`${id}_gain`);
  const ratioSliderState = Juce.getSliderState(`${id}_ratio`);

  useEffect(() => {
    const updateGainSlider = () => {
      const updatedValue = gainSliderState.getNormalisedValue();
      const normalisedValue = updatedValue;
      const roundedValue = Math.round(normalisedValue * 10000) / 10000;
      onGainChange(id, roundedValue);
    };
    const updateRatioSlider = () => {
      const updatedValue = ratioSliderState.getNormalisedValue();
      const normalisedValue = updatedValue;
      const roundedValue = Math.round(normalisedValue * 10000) / 10000;
      onRatioChange(id, roundedValue * (maxHarmonic - minHarmonic));
    };
    updateGainSlider();
    updateRatioSlider();
    gainSliderState.valueChangedEvent.addListener(updateGainSlider);
    ratioSliderState.valueChangedEvent.addListener(updateRatioSlider);
  }, [id, onGainChange, gainSliderState, ratioSliderState, onRatioChange]);

  const calculateGainFromY = useCallback(
    (clientY: number) => {
      if (!containerRef.current) return gain;
      const rect = containerRef.current.getBoundingClientRect();
      const relativeY = clientY - rect.top;
      const height = rect.height;
      // Clamp between 0 and 1, inverted (top = 1, bottom = 0)
      const newGain = Math.max(0, Math.min(1, 1 - relativeY / height));
      return newGain;
    },
    [gain]
  );

  const calculateRatioFromX = useCallback(
    (clientX: number) => {
      if (!containerRef.current) return ratio;
      const relativeX = clientX - wrapperLeft;
      const newRatio = Math.max(
        0,
        Math.min(maxHarmonic, relativeX / (wrapperWidth / maxHarmonic))
      );
      return newRatio;
    },
    [ratio, wrapperWidth, wrapperLeft]
  );

  const handleMouseDown = useCallback(
    (e: React.MouseEvent<HTMLDivElement>) => {
      e.preventDefault();

      if (e.shiftKey) {
        setIsDragging(2);
      } else {
        setIsDragging(1);
        const newGain = calculateGainFromY(e.clientY);
        onGainChange(id, newGain);
        gainSliderState.setNormalisedValue(newGain);
      }
    },
    [id, onGainChange, calculateGainFromY, gainSliderState]
  );

  useEffect(() => {
    let hoverCount = 0;
    const container = containerRef.current;
    const span = spanRef.current;

    const handleContainerEnter = () => {
      hoverCount++;
      setIsHovering(true);
    };
    const handleContainerLeave = () => {
      hoverCount--;
      if (hoverCount <= 0) {
        hoverCount = 0;
        setIsHovering(false);
      }
    };
    const handleSpanEnter = () => {
      hoverCount++;
      setIsHovering(true);
    };
    const handleSpanLeave = () => {
      hoverCount--;
      if (hoverCount <= 0) {
        hoverCount = 0;
        setIsHovering(false);
      }
    };

    container?.addEventListener("mouseenter", handleContainerEnter);
    container?.addEventListener("mouseleave", handleContainerLeave);
    span?.addEventListener("mouseenter", handleSpanEnter);
    span?.addEventListener("mouseleave", handleSpanLeave);

    return () => {
      container?.removeEventListener("mouseenter", handleContainerEnter);
      container?.removeEventListener("mouseleave", handleContainerLeave);
      span?.removeEventListener("mouseenter", handleSpanEnter);
      span?.removeEventListener("mouseleave", handleSpanLeave);
    };
  }, []);

  useEffect(() => {
    if (!isDragging) return;

    const handleMouseMove = (e: MouseEvent) => {
      if (isDragging === 1) {
        const newGain = calculateGainFromY(e.clientY);
        onGainChange(id, newGain);
        gainSliderState.setNormalisedValue(newGain);
      } else if (isDragging === 2) {
        const newRatio = calculateRatioFromX(e.clientX);
        const updatedRatio = !e.shiftKey
          ? Math.round(newRatio * 100) / 100
          : Math.round(newRatio);
        onRatioChange(id, updatedRatio);
        ratioSliderState.setNormalisedValue(
          updatedRatio / (maxHarmonic - minHarmonic)
        );
      }
    };

    const handleMouseUp = () => {
      setIsDragging(0);
    };

    window.addEventListener("mousemove", handleMouseMove);
    window.addEventListener("mouseup", handleMouseUp);

    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", handleMouseUp);
    };
  }, [
    isDragging,
    id,
    onGainChange,
    calculateGainFromY,
    onRatioChange,
    calculateRatioFromX,
    gainSliderState,
    ratioSliderState,
  ]);

  const showRatioLabel = isHovering || isDragging !== 0;

  return (
    <div
      ref={containerRef}
      className="absolute w-[8px] h-full flex items-end justify-center translate-x-[-4px]"
      style={{
        left: `${(ratio / maxHarmonic) * 100}%`,
      }}
      onMouseDown={handleMouseDown}
    >
      <div className="size-full bg-black/10 m-0.5 flex items-end">
        <div
          className="w-full bg-black/70 transition-opacity"
          style={{
            height: `${gain * 100}%`,
            opacity: isDragging ? 0.8 : 1,
          }}
        />
      </div>
      <span
        ref={spanRef}
        className="text-[12px] bg-blue-500 text-black absolute bottom-[-16px] text-center transition-opacity duration-200"
        style={{ opacity: showRatioLabel ? 1 : 0 }}
      >
        {ratio.toFixed(2)}
      </span>
    </div>
  );
};

const Harmonics = () => {
  const harmonicsWrapperRef = useRef<HTMLDivElement>(null);
  const [harmonics, setHarmonics] = useState([
    {
      id: "h1",
      ratio: 1,
      gain: 0.5,
    },
    {
      id: "h2",
      ratio: 2,
      gain: 0.5,
    },
    {
      id: "h3",
      ratio: 3,
      gain: 0.5,
    },
    {
      id: "h4",
      ratio: 4,
      gain: 0.5,
    },
    {
      id: "h5",
      ratio: 5,
      gain: 0.5,
    },
    {
      id: "h6",
      ratio: 6,
      gain: 0.5,
    },
    {
      id: "h7",
      ratio: 7,
      gain: 0.5,
    },
    {
      id: "h8",
      ratio: 8,
      gain: 0.5,
    },
  ]);

  const handleGainChange = useCallback((id: string, newGain: number) => {
    setHarmonics((prev) =>
      prev.map((harmonic) =>
        harmonic.id === id ? { ...harmonic, gain: newGain } : harmonic
      )
    );
  }, []);

  const handleRatioChange = useCallback((id: string, newRatio: number) => {
    setHarmonics((prev) =>
      prev.map((harmonic) =>
        harmonic.id === id ? { ...harmonic, ratio: newRatio } : harmonic
      )
    );
  }, []);

  return (
    <div className="w-[752px] h-[124px] bg-black/10 p-6">
      <div ref={harmonicsWrapperRef} className="relative w-full h-full">
        {harmonics.map((harmonic) => (
          <Harmonic
            key={harmonic.id}
            id={harmonic.id}
            ratio={harmonic.ratio}
            gain={harmonic.gain}
            harmonicsWrapperBounds={harmonicsWrapperRef.current?.getBoundingClientRect()}
            onGainChange={handleGainChange}
            onRatioChange={handleRatioChange}
          />
        ))}
      </div>
    </div>
  );
};

export default Harmonics;
