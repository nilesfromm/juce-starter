import { useRef, useState, useEffect, useCallback } from "react";
import { useParamStore } from "../../store/paramStore";
import { useUIStore } from "../../store/uiStore";

const ALL_VOICES = ["h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8", "h9"];

function computePoints(
  attack: number,
  decay: number,
  sustain: number,
  release: number,
  w: number,
  h: number,
  pad: number,
) {
  const attackZone = w * 0.3;
  const decayZone = w * 0.25;
  const releaseZone = w * 0.3 - pad;

  const p0x = pad;
  const p0y = h - pad;
  const p1x = pad + (attack * attackZone) / 100;
  const p1y = pad;
  const p2x = p1x + (decay * decayZone) / 100;
  const p2y = pad + ((100 - sustain) * (h - pad * 2)) / 100;
  const p3x = w * 0.7;
  const p3y = p2y;
  const p4x = p3x + (release * releaseZone) / 100;
  const p4y = h - pad;

  return [
    { x: p0x, y: p0y },
    { x: p1x, y: p1y },
    { x: p2x, y: p2y },
    { x: p3x, y: p3y },
    { x: p4x, y: p4y },
  ];
}

const EnvelopePath = ({
  voiceId,
  w,
  h,
  pad,
}: {
  voiceId: string;
  w: number;
  h: number;
  pad: number;
}) => {
  const attack = useParamStore((s) => s.values[`${voiceId}_attack`] ?? 0);
  const decay = useParamStore((s) => s.values[`${voiceId}_decay`] ?? 0);
  const sustain = useParamStore((s) => s.values[`${voiceId}_sustain`] ?? 0);
  const release = useParamStore((s) => s.values[`${voiceId}_release`] ?? 0);
  const gain = useParamStore((s) => s.values[`${voiceId}_gain`] ?? 0);
  const setSelectedVoice = useUIStore((s) => s.setSelectedVoice);

  const points = computePoints(attack, decay, sustain, release, w, h, pad);
  const d = `M ${points.map((p) => `${p.x} ${p.y}`).join(" L ")}`;

  return (
    <g
      style={{ cursor: gain > 0.0001 ? "pointer" : "default" }}
      onClick={() => gain > 0.0001 && setSelectedVoice(voiceId)}
    >
      {/* Invisible wider hit area */}
      <path d={d} fill="none" stroke="transparent" strokeWidth="12" />
      <path
        d={d}
        fill="none"
        stroke="#999"
        strokeWidth="1.5"
        opacity={gain * 0.5}
      />
    </g>
  );
};

interface EnvelopeProps {
  voiceId: string;
}

const Envelope = ({ voiceId }: EnvelopeProps) => {
  const attack = useParamStore((s) => s.values[`${voiceId}_attack`] ?? 0);
  const decay = useParamStore((s) => s.values[`${voiceId}_decay`] ?? 0);
  const sustain = useParamStore((s) => s.values[`${voiceId}_sustain`] ?? 0);
  const release = useParamStore((s) => s.values[`${voiceId}_release`] ?? 0);
  const gain = useParamStore((s) => s.values[`${voiceId}_gain`] ?? 0);
  const setParam = useParamStore((s) => s.setParam);

  const containerRef = useRef<HTMLDivElement>(null);
  const [size, setSize] = useState({ w: 0, h: 0 });
  const [dragging, setDragging] = useState<number | null>(null);

  useEffect(() => {
    const el = containerRef.current;
    if (!el) return;
    const observer = new ResizeObserver(() => {
      setSize({ w: el.clientWidth, h: el.clientHeight });
    });
    observer.observe(el);
    return () => observer.disconnect();
  }, []);

  const { w, h } = size;
  const pad = 12;

  const attackZone = w * 0.3;
  const decayZone = w * 0.25;
  const releaseZone = w * 0.3 - pad;

  const pathPoints = computePoints(attack, decay, sustain, release, w, h, pad);

  const handles = [
    { x: pathPoints[1].x, y: pathPoints[1].y },
    { x: pathPoints[2].x, y: pathPoints[2].y },
    { x: pathPoints[4].x, y: pathPoints[4].y },
  ];

  const clamp = (v: number, min: number, max: number) =>
    Math.max(min, Math.min(max, v));

  const handleMouseDown = useCallback(
    (handleIndex: number) => (e: React.MouseEvent) => {
      e.preventDefault();
      e.stopPropagation();
      setDragging(handleIndex);
    },
    [],
  );

  useEffect(() => {
    if (dragging === null) return;

    const handleMouseMove = (e: MouseEvent) => {
      const el = containerRef.current;
      if (!el) return;
      const rect = el.getBoundingClientRect();
      const mx = e.clientX - rect.left;
      const my = e.clientY - rect.top;

      if (dragging === 0) {
        const val = clamp(((mx - pad) / attackZone) * 100, 0, 100);
        setParam(`${voiceId}_attack`, Math.round(val));
      } else if (dragging === 1) {
        const attackX = pad + (attack * attackZone) / 100;
        const val = clamp(((mx - attackX) / decayZone) * 100, 0, 100);
        setParam(`${voiceId}_decay`, Math.round(val));
        const susVal = clamp(((h - pad - my) / (h - pad * 2)) * 100, 0, 100);
        setParam(`${voiceId}_sustain`, Math.round(susVal));
      } else if (dragging === 2) {
        const sx = w * 0.7;
        const val = clamp(((mx - sx) / releaseZone) * 100, 0, 100);
        setParam(`${voiceId}_release`, Math.round(val));
      }
    };

    const handleMouseUp = () => setDragging(null);

    window.addEventListener("mousemove", handleMouseMove);
    window.addEventListener("mouseup", handleMouseUp);
    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", handleMouseUp);
    };
  }, [
    dragging,
    voiceId,
    attack,
    w,
    h,
    pad,
    attackZone,
    decayZone,
    releaseZone,
    setParam,
  ]);

  const backgroundVoices = ALL_VOICES.filter((id) => id !== voiceId);

  return (
    <div ref={containerRef} className="w-full h-[140px] bg-[#D9D9D9] rounded">
      {w > 0 && (
        <svg width={w} height={h}>
          {/* Background envelopes */}
          {backgroundVoices.map((id) => (
            <EnvelopePath key={id} voiceId={id} w={w} h={h} pad={pad} />
          ))}
          {/* Selected envelope */}
          <path
            d={`M ${pathPoints.map((p) => `${p.x} ${p.y}`).join(" L ")}`}
            fill="none"
            stroke="#555"
            strokeWidth="1.5"
            opacity={0.25 + gain * 0.75}
          />
          {handles.map((p, i) => (
            <circle
              key={i}
              cx={p.x}
              cy={p.y}
              r="5"
              fill={dragging === i ? "#ddd" : "#D9D9D9"}
              stroke="#555"
              strokeWidth="1.5"
              className="cursor-grab active:cursor-grabbing"
              onMouseDown={handleMouseDown(i)}
            />
          ))}
        </svg>
      )}
    </div>
  );
};

export default Envelope;
