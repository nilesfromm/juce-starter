import { useState, useCallback, useRef, useEffect } from "react";
import { useParamStore } from "../../store/paramStore";
import { useUIStore } from "../../store/uiStore";

interface HarmonicSliderProps {
  id: string;
  label: string;
}

const HarmonicSlider = ({ id, label }: HarmonicSliderProps) => {
  const gain = useParamStore((s) => s.values[`${id}_gain`] ?? 0);
  const setParam = useParamStore((s) => s.setParam);
  const selectedVoice = useUIStore((s) => s.selectedVoice);
  const setSelectedVoice = useUIStore((s) => s.setSelectedVoice);

  const [isDragging, setIsDragging] = useState(false);
  const containerRef = useRef<HTMLDivElement>(null);
  const isSelected = selectedVoice === id;

  const calculateGainFromY = useCallback(
    (clientY: number) => {
      if (!containerRef.current) return gain;
      const rect = containerRef.current.getBoundingClientRect();
      const relativeY = clientY - rect.top;
      const height = rect.height;
      return Math.max(0, Math.min(1, 1 - relativeY / height));
    },
    [gain],
  );

  const handleMouseDown = useCallback(
    (e: React.MouseEvent<HTMLDivElement>) => {
      e.preventDefault();
      setSelectedVoice(id);
      setIsDragging(true);
      // const newGain = calculateGainFromY(e.clientY);
      // setParam(`${id}_gain`, newGain);
    },
    [id, calculateGainFromY, setParam, setSelectedVoice],
  );

  useEffect(() => {
    if (!isDragging) return;

    const handleMouseMove = (e: MouseEvent) => {
      setParam(`${id}_gain`, calculateGainFromY(e.clientY));
    };

    const handleMouseUp = () => setIsDragging(false);

    window.addEventListener("mousemove", handleMouseMove);
    window.addEventListener("mouseup", handleMouseUp);
    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", handleMouseUp);
    };
  }, [isDragging, id, calculateGainFromY, setParam]);

  return (
    <div className="flex flex-col items-center gap-1">
      <div
        ref={containerRef}
        className="w-full h-full flex items-end cursor-ns-resize transition-opacity"
        style={{ opacity: isSelected ? 1 : 0.5 }}
        onMouseDown={handleMouseDown}
      >
        <div className="size-full bg-black/10 flex items-end">
          <div
            className="w-full bg-black/50"
            style={{ height: `${gain * 100}%` }}
          />
        </div>
      </div>
      <span
        className="text-[11px] select-none"
        style={{ fontWeight: isSelected ? 700 : 400 }}
      >
        {label}
      </span>
    </div>
  );
};

const HARMONICS = [
  { id: "h1", label: "1" },
  { id: "h2", label: "2" },
  { id: "h3", label: "3" },
  { id: "h4", label: "4" },
  { id: "h5", label: "5" },
  { id: "h6", label: "6" },
  { id: "h7", label: "7" },
  { id: "h8", label: "8" },
  { id: "h9", label: "N" },
];

const Harmonics = () => {
  return (
    <div className="w-full h-[124px] bg-black/5 p-2 pt-3 pb-0">
      <div className="grid grid-cols-9 gap-1 h-full">
        {HARMONICS.map(({ id, label }) => (
          <HarmonicSlider key={id} id={id} label={label} />
        ))}
      </div>
    </div>
  );
};

export default Harmonics;
