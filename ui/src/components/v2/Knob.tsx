import { useRef, useCallback } from "react";
import { useParamStore } from "../../store/paramStore";

interface KnobProps {
  id: string;
  min: number;
  max: number;
  step: number;
  size?: number;
}

const START_ANGLE = 225;
const END_ANGLE = -45;
const SWEEP = START_ANGLE - END_ANGLE; // 270 degrees

const Knob = ({ id, min, max, step, size = 32 }: KnobProps) => {
  const value = useParamStore((s) => s.values[id] ?? 0);
  const setParam = useParamStore((s) => s.setParam);
  const currentValueRef = useRef(0);

  const normalized = (value - min) / (max - min);
  const angle = START_ANGLE - normalized * SWEEP;
  const angleRad = (angle * Math.PI) / 180;

  const cx = size / 2;
  const cy = size / 2;
  const r = size / 2 - 3;
  const notchInner = r * 0.45;
  const notchOuter = r * 0.85;

  const handleMouseDown = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault();
      currentValueRef.current = value;

      const handleMouseMove = (e: MouseEvent) => {
        e.preventDefault();
        const delta = -e.movementY * step;
        currentValueRef.current = Math.max(
          min,
          Math.min(max, currentValueRef.current + delta),
        );
        setParam(id, Number(currentValueRef.current.toFixed(2)));
      };

      const handleMouseUp = () => {
        document.removeEventListener("mousemove", handleMouseMove);
        document.removeEventListener("mouseup", handleMouseUp);
      };

      document.addEventListener("mousemove", handleMouseMove);
      document.addEventListener("mouseup", handleMouseUp);
    },
    [id, value, min, max, step, setParam],
  );

  return (
    <svg
      width={size}
      height={size}
      viewBox={`0 0 ${size} ${size}`}
      className="cursor-ns-resize"
      onMouseDown={handleMouseDown}
    >
      {/* Track arc */}
      <circle
        cx={cx}
        cy={cy}
        r={r}
        fill="#D9D9D9"
        stroke="#BBB"
        strokeWidth="1"
      />
      {/* Notch indicator */}
      <line
        x1={cx + Math.cos(angleRad) * notchInner}
        y1={cy - Math.sin(angleRad) * notchInner}
        x2={cx + Math.cos(angleRad) * notchOuter}
        y2={cy - Math.sin(angleRad) * notchOuter}
        stroke="#555"
        strokeWidth="2"
        strokeLinecap="round"
      />
    </svg>
  );
};

export default Knob;
