import { useRef, useState, useCallback, useEffect } from "react";
import { useParamStore } from "../../store/paramStore";

interface SpectrogramRowProps {
  id: string;
  label: string;
}

const STAGE_COLORS = {
  base: [235, 235, 235],
  attack: [45, 100, 180],
  decay: [45, 100, 180],
  sustain: [45, 100, 180],
  release: [45, 100, 180],
} as const;

function lerpColor(rgb: readonly [number, number, number], t: number): string {
  const base = STAGE_COLORS.base;
  const r = Math.round(base[0] + (rgb[0] - base[0]) * t);
  const g = Math.round(base[1] + (rgb[1] - base[1]) * t);
  const b = Math.round(base[2] + (rgb[2] - base[2]) * t);
  return `rgb(${r},${g},${b})`;
}

const MAX_TIME = 100;

const SpectrogramRow = ({ id, label }: SpectrogramRowProps) => {
  const gain = useParamStore((s) => s.values[`${id}_gain`] ?? 0);
  const attack = useParamStore((s) => s.values[`${id}_attack`] ?? 0);
  const decay = useParamStore((s) => s.values[`${id}_decay`] ?? 0);
  const sustain = useParamStore((s) => s.values[`${id}_sustain`] ?? 50);
  const release = useParamStore((s) => s.values[`${id}_release`] ?? 0);
  const setParam = useParamStore((s) => s.setParam);

  const canvasRef = useRef<HTMLCanvasElement>(null);
  const containerRef = useRef<HTMLDivElement>(null);

  // Drag types: gain (vertical), sustain (vertical on sustain region),
  // divider (A|D or D|S), release-end (trailing edge handle)
  type DragType = "gain" | "sustain" | "divider" | "release-end";
  const [dragState, setDragState] = useState<{
    type: DragType;
    dividerIndex?: number;
    startY?: number;
    startValue?: number;
  } | null>(null);

  const getStageWidths = useCallback(
    (width: number) => {
      const quarter = width * 0.25;
      const aW = (attack / MAX_TIME) * quarter;
      const dW = (decay / MAX_TIME) * quarter;
      const sW = quarter;
      const rW = (release / MAX_TIME) * quarter;
      return { aW, dW, sW, rW, total: aW + dW + sW + rW };
    },
    [attack, decay, release],
  );

  // Draw the spectrogram row
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext("2d");
    if (!ctx) return;

    const rect = canvas.getBoundingClientRect();
    const dpr = window.devicePixelRatio || 1;
    canvas.width = rect.width * dpr;
    canvas.height = rect.height * dpr;
    ctx.scale(dpr, dpr);

    const w = rect.width;
    const h = rect.height;
    const { aW, dW, sW, rW, total } = getStageWidths(w);

    ctx.clearRect(0, 0, w, h);

    ctx.fillStyle = `rgb(${STAGE_COLORS.base.join(",")})`;
    ctx.fillRect(0, 0, w, h);

    const sustainLevel = sustain / 100;

    // Attack: gradient from 0 to gain
    if (aW > 0) {
      const attackGrad = ctx.createLinearGradient(0, 0, aW, 0);
      attackGrad.addColorStop(0, lerpColor(STAGE_COLORS.base, 1));
      attackGrad.addColorStop(1, lerpColor(STAGE_COLORS.attack, gain));
      ctx.fillStyle = attackGrad;
      ctx.fillRect(0, 0, aW, h);
    }

    // Decay: gradient from gain to sustain*gain
    if (dW > 0) {
      const decayStart = aW;
      const decayGrad = ctx.createLinearGradient(
        decayStart,
        0,
        decayStart + dW,
        0,
      );
      decayGrad.addColorStop(0, lerpColor(STAGE_COLORS.decay, gain));
      decayGrad.addColorStop(
        1,
        lerpColor(STAGE_COLORS.decay, gain * sustainLevel),
      );
      ctx.fillStyle = decayGrad;
      ctx.fillRect(decayStart, 0, dW, h);
    }

    // Sustain: flat at sustain*gain
    const sustainStart = aW + dW;
    ctx.fillStyle = lerpColor(STAGE_COLORS.sustain, gain * sustainLevel);
    ctx.fillRect(sustainStart, 0, sW, h);

    // Release: gradient from sustain*gain to 0
    if (rW > 0) {
      const releaseStart = sustainStart + sW;
      const releaseGrad = ctx.createLinearGradient(
        releaseStart,
        0,
        releaseStart + rW,
        0,
      );
      releaseGrad.addColorStop(
        0,
        lerpColor(STAGE_COLORS.release, gain * sustainLevel),
      );
      releaseGrad.addColorStop(1, lerpColor(STAGE_COLORS.release, 0));
      ctx.fillStyle = releaseGrad;
      ctx.fillRect(releaseStart, 0, rW, h);
    }

    // Draw divider lines between A|D and D|S
    ctx.strokeStyle = `rgba(59, 122, 203,${gain * 0})`;
    ctx.lineWidth = 2;
    for (const x of [aW, aW + dW, aW + dW + sW]) {
      ctx.beginPath();
      ctx.moveTo(x, 0);
      ctx.lineTo(x, h);
      ctx.stroke();
    }

    // Draw release end handle (brighter line at the trailing edge)
    ctx.strokeStyle = `rgba(59, 122, 203,${gain * 0})`;
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(total, 0);
    ctx.lineTo(total, h);
    ctx.stroke();
  }, [gain, attack, decay, sustain, release, getStageWidths]);

  // Hit-test: returns "divider:0", "divider:1", "release-end", "sustain", or "body"
  const hitTest = useCallback(
    (clientX: number): { zone: string; dividerIndex?: number } => {
      const canvas = canvasRef.current;
      if (!canvas) return { zone: "body" };
      const rect = canvas.getBoundingClientRect();
      const x = clientX - rect.left;
      const w = rect.width;
      const { aW, dW, sW, total } = getStageWidths(w);

      const threshold = 8;

      // Check release end handle first (trailing edge)
      if (Math.abs(x - total) < threshold) return { zone: "release-end" };

      // Check A|D and D|S dividers (index 0 and 1 only)
      const dividers = [aW, aW + dW];
      for (let i = 0; i < dividers.length; i++) {
        if (Math.abs(x - dividers[i]) < threshold)
          return { zone: "divider", dividerIndex: i };
      }

      // Check sustain region
      const sustainStart = aW + dW;
      if (x >= sustainStart && x <= sustainStart + sW)
        return { zone: "sustain" };

      return { zone: "body" };
    },
    [getStageWidths],
  );

  const handleMouseDown = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault();
      const { zone, dividerIndex } = hitTest(e.clientX);
      if (zone === "divider") {
        setDragState({ type: "divider", dividerIndex });
      } else if (zone === "release-end") {
        setDragState({ type: "release-end" });
      } else if (zone === "sustain") {
        setDragState({
          type: "sustain",
          startY: e.clientY,
          startValue: sustain,
        });
      } else {
        setDragState({ type: "gain", startY: e.clientY, startValue: gain });
      }
    },
    [hitTest, gain, sustain],
  );

  useEffect(() => {
    if (!dragState) return;

    const handleMouseMove = (e: MouseEvent) => {
      if (dragState.type === "gain") {
        const dy = dragState.startY! - e.clientY;
        const sensitivity = 200;
        const newGain = Math.max(
          0,
          Math.min(1, dragState.startValue! + dy / sensitivity),
        );
        setParam(`${id}_gain`, Math.round(newGain * 100) / 100);
      } else if (dragState.type === "sustain") {
        const dy = dragState.startY! - e.clientY;
        const sensitivity = 2;
        const newSustain = Math.max(
          0,
          Math.min(100, dragState.startValue! + dy * sensitivity),
        );
        setParam(`${id}_sustain`, Math.round(newSustain));
      } else if (dragState.type === "release-end") {
        // Drag the trailing edge: distance from sustain end = release width
        const canvas = canvasRef.current;
        if (!canvas) return;
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const w = rect.width;
        const quarter = w * 0.25;
        const { aW, dW, sW } = getStageWidths(w);
        const sustainEnd = aW + dW + sW;
        const releasePx = x - sustainEnd;
        const newRelease = Math.round((releasePx / quarter) * MAX_TIME);
        setParam(`${id}_release`, Math.min(MAX_TIME, Math.max(0, newRelease)));
      } else if (dragState.type === "divider") {
        const canvas = canvasRef.current;
        if (!canvas) return;
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const w = rect.width;
        const quarter = w * 0.25;

        const idx = dragState.dividerIndex!;
        if (idx === 0) {
          const newAttack = Math.round((x / quarter) * MAX_TIME);
          setParam(`${id}_attack`, Math.min(MAX_TIME, Math.max(0, newAttack)));
        } else if (idx === 1) {
          const { aW } = getStageWidths(w);
          const newDecay = Math.round(((x - aW) / quarter) * MAX_TIME);
          setParam(`${id}_decay`, Math.min(MAX_TIME, Math.max(0, newDecay)));
        }
      }
    };

    const handleMouseUp = () => setDragState(null);

    window.addEventListener("mousemove", handleMouseMove);
    window.addEventListener("mouseup", handleMouseUp);
    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", handleMouseUp);
    };
  }, [dragState, id, setParam, getStageWidths]);

  // Cursor style based on hover position
  const [cursor, setCursor] = useState("ns-resize");
  const handleMouseMoveLocal = useCallback(
    (e: React.MouseEvent) => {
      if (dragState) return;
      const { zone } = hitTest(e.clientX);
      setCursor(
        zone === "divider" || zone === "release-end"
          ? "ew-resize"
          : "ns-resize",
      );
    },
    [hitTest, dragState],
  );

  return (
    <div
      ref={containerRef}
      className="flex items-center gap-2"
      style={{ height: "100%" }}
    >
      <span className="text-[11px] text-[#999] w-6 text-right select-none shrink-0">
        {label}
      </span>
      <canvas
        ref={canvasRef}
        className="w-full h-full rounded-sm"
        style={{ cursor, background: "#FFF" }}
        onMouseDown={handleMouseDown}
        onMouseMove={handleMouseMoveLocal}
      />
    </div>
  );
};

export default SpectrogramRow;
