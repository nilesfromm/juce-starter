import { useRef, useCallback } from "react";
import { useParamStore } from "../store/paramStore";

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
  const value = useParamStore((s) => s.values[id] ?? 0);
  const setParam = useParamStore((s) => s.setParam);
  const isDragging = useRef(false);
  const currentValueRef = useRef(0);

  const handleChange = useCallback(
    (newValue: number) => {
      const constrained = Math.max(min, Math.min(max, newValue));
      setParam(id, constrained);
      if (onChange) onChange(constrained);
    },
    [id, min, max, setParam, onChange],
  );

  const handleMouseDown = useCallback(
    (e: React.MouseEvent<HTMLButtonElement>) => {
      isDragging.current = true;
      currentValueRef.current = value;

      const handleMouseMove = (e: MouseEvent) => {
        e.preventDefault();
        if (!isDragging.current) return;
        const sensitivity = step;
        const delta = -e.movementY * sensitivity;
        currentValueRef.current = Number(
          Math.max(min, Math.min(max, currentValueRef.current + delta)).toFixed(
            2,
          ),
        );
        handleChange(currentValueRef.current);
      };

      const handleMouseUp = (e: MouseEvent) => {
        e.preventDefault();
        isDragging.current = false;
        document.removeEventListener("mousemove", handleMouseMove);
        document.removeEventListener("mouseup", handleMouseUp);
      };

      document.addEventListener("mousemove", handleMouseMove);
      document.addEventListener("mouseup", handleMouseUp);
    },
    [value, min, max, step, handleChange],
  );

  return (
    <div className="flex text-[12px]">
      <button
        className="size-6 bg-[#333333] flex items-center justify-center"
        onMouseDown={handleMouseDown}
      >
        <span className="text-white">{label}</span>
      </button>
      <input
        type="number"
        className="bg-[#D9D9D9] h-6 w-full text-center"
        value={isDragging.current ? currentValueRef.current : value}
        onChange={(e) => handleChange(Number(e.target.value))}
        onKeyDown={(e) => {
          if (e.key === "Enter") {
            handleChange(Number((e.target as HTMLInputElement).value));
          }
        }}
      />
    </div>
  );
};

export default NumberInput;
