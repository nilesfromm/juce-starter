import { useState, useCallback, useRef, useEffect } from "react";
import { useParamStore } from "../store/paramStore";

interface HarmonicProps {
  id: string;
  harmonicsWrapperBounds: DOMRect | undefined;
}

const Harmonic = ({ id, harmonicsWrapperBounds }: HarmonicProps) => {
  const minHarmonic = 0;
  const maxHarmonic = 16;

  const gain = useParamStore((s) => s.values[`${id}_gain`] ?? 0);
  const ratio = useParamStore((s) => s.values[`${id}_ratio`] ?? 0);
  const setParam = useParamStore((s) => s.setParam);

  const [isDragging, setIsDragging] = useState<0 | 1 | 2>(0);
  const [isHovering, setIsHovering] = useState(false);
  const containerRef = useRef<HTMLDivElement>(null);
  const spanRef = useRef<HTMLSpanElement>(null);
  const { width: wrapperWidth, left: wrapperLeft } = harmonicsWrapperBounds || {
    width: 0,
    left: 0,
  };

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

  const calculateRatioFromX = useCallback(
    (clientX: number) => {
      if (!containerRef.current) return ratio;
      const relativeX = clientX - wrapperLeft;
      return Math.max(
        minHarmonic,
        Math.min(
          maxHarmonic,
          minHarmonic +
            relativeX / (wrapperWidth / (maxHarmonic - minHarmonic)),
        ),
      );
    },
    [ratio, wrapperWidth, wrapperLeft],
  );

  const handleMouseDown = useCallback(
    (e: React.MouseEvent<HTMLDivElement>) => {
      e.preventDefault();
      if (e.shiftKey) {
        setIsDragging(2);
      } else {
        setIsDragging(1);
        const newGain = calculateGainFromY(e.clientY);
        setParam(`${id}_gain`, newGain);
      }
    },
    [id, calculateGainFromY, setParam],
  );

  useEffect(() => {
    // TODO: take this logic out if not needed?
    // let hoverCount = 0;
    const container = containerRef.current;
    const span = spanRef.current;

    const handleEnter = () => {
      // hoverCount++;
      setIsHovering(true);
    };
    const handleLeave = () => {
      // hoverCount--;
      // if (hoverCount <= 0) {
      //   hoverCount = 0;
      setIsHovering(false);
      // }
    };

    container?.addEventListener("mouseenter", handleEnter);
    container?.addEventListener("mouseleave", handleLeave);
    span?.addEventListener("mouseenter", handleEnter);
    span?.addEventListener("mouseleave", handleLeave);

    return () => {
      container?.removeEventListener("mouseenter", handleEnter);
      container?.removeEventListener("mouseleave", handleLeave);
      span?.removeEventListener("mouseenter", handleEnter);
      span?.removeEventListener("mouseleave", handleLeave);
    };
  }, []);

  useEffect(() => {
    if (!isDragging) return;

    const handleMouseMove = (e: MouseEvent) => {
      if (isDragging === 1) {
        setParam(`${id}_gain`, calculateGainFromY(e.clientY));
      } else if (isDragging === 2) {
        const newRatio = calculateRatioFromX(e.clientX);
        const updatedRatio = !e.shiftKey
          ? Math.round(newRatio * 100) / 100
          : Math.round(newRatio);
        setParam(`${id}_ratio`, updatedRatio);
      }
    };

    const handleMouseUp = () => setIsDragging(0);

    window.addEventListener("mousemove", handleMouseMove);
    window.addEventListener("mouseup", handleMouseUp);
    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", handleMouseUp);
    };
  }, [isDragging, id, calculateGainFromY, calculateRatioFromX, setParam]);

  const showRatioLabel = isHovering || isDragging !== 0;

  return (
    <div
      ref={containerRef}
      className="absolute w-[8px] h-full flex items-end justify-center translate-x-[-4px]"
      style={{
        left: `${((ratio - minHarmonic) / (maxHarmonic - minHarmonic)) * 100}%`,
      }}
      onMouseDown={handleMouseDown}
    >
      <div className="size-full bg-black/10 m-0.5 flex items-end">
        <div
          className="w-full bg-black/70 transition-opacity"
          style={{ height: `${gain * 100}%`, opacity: isDragging ? 0.8 : 1 }}
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

const HARMONIC_IDS = ["h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8"];

const Harmonics = () => {
  const harmonicsWrapperRef = useRef<HTMLDivElement>(null);
  const [wrapperBounds, setWrapperBounds] = useState<DOMRect | undefined>();

  useEffect(() => {
    const wrapperRef = harmonicsWrapperRef.current;
    if (!wrapperRef) return;

    const observer = new ResizeObserver(() => {
      setWrapperBounds(wrapperRef.getBoundingClientRect());
    });
    observer.observe(wrapperRef);
    return () => observer.disconnect();
  }, []);

  return (
    <div className="w-full h-[124px] bg-black/10 p-6">
      <div ref={harmonicsWrapperRef} className="relative w-full h-full">
        {HARMONIC_IDS.map((id) => (
          <Harmonic key={id} id={id} harmonicsWrapperBounds={wrapperBounds} />
        ))}
      </div>
    </div>
  );
};

export default Harmonics;
