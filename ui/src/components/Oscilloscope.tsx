import { useEffect, useRef, useMemo } from "react";
import { useParamStore } from "../store/paramStore";

const Oscilloscope = () => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const h1_gain = useParamStore((s) => s.values[`h1_gain`] ?? 0);
  const h1_ratio = useParamStore((s) => s.values[`h1_ratio`] ?? 0);
  const h2_gain = useParamStore((s) => s.values[`h2_gain`] ?? 0);
  const h2_ratio = useParamStore((s) => s.values[`h2_ratio`] ?? 0);
  const h3_gain = useParamStore((s) => s.values[`h3_gain`] ?? 0);
  const h3_ratio = useParamStore((s) => s.values[`h3_ratio`] ?? 0);
  const h4_gain = useParamStore((s) => s.values[`h4_gain`] ?? 0);
  const h4_ratio = useParamStore((s) => s.values[`h4_ratio`] ?? 0);
  const h5_gain = useParamStore((s) => s.values[`h5_gain`] ?? 0);
  const h5_ratio = useParamStore((s) => s.values[`h5_ratio`] ?? 0);
  const h6_gain = useParamStore((s) => s.values[`h6_gain`] ?? 0);
  const h6_ratio = useParamStore((s) => s.values[`h6_ratio`] ?? 0);
  const h7_gain = useParamStore((s) => s.values[`h7_gain`] ?? 0);
  const h7_ratio = useParamStore((s) => s.values[`h7_ratio`] ?? 0);
  const h8_gain = useParamStore((s) => s.values[`h8_gain`] ?? 0);
  const h8_ratio = useParamStore((s) => s.values[`h8_ratio`] ?? 0);

  const harmonicData = useMemo(
    () => [
      { gain: h1_gain, ratio: h1_ratio },
      { gain: h2_gain, ratio: h2_ratio },
      { gain: h3_gain, ratio: h3_ratio },
      { gain: h4_gain, ratio: h4_ratio },
      { gain: h5_gain, ratio: h5_ratio },
      { gain: h6_gain, ratio: h6_ratio },
      { gain: h7_gain, ratio: h7_ratio },
      { gain: h8_gain, ratio: h8_ratio },
    ],
    [
      h1_gain,
      h1_ratio,
      h2_gain,
      h2_ratio,
      h3_gain,
      h3_ratio,
      h4_gain,
      h4_ratio,
      h5_gain,
      h5_ratio,
      h6_gain,
      h6_ratio,
      h7_gain,
      h7_ratio,
      h8_gain,
      h8_ratio,
    ],
  );

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
    const samples = Math.floor(w);

    // Find max amplitude for scaling
    let maxAmp = 0;
    for (let i = 0; i < samples; i++) {
      const t = (i / samples - 0.5) * 2 * Math.PI;
      let sample = 0;
      for (const { gain, ratio } of harmonicData) {
        if (gain > 0 && ratio > 0) {
          sample += gain * Math.sin(t * ratio);
        }
      }
      maxAmp = Math.max(maxAmp, Math.abs(sample));
    }

    // Only normalize if clipping; otherwise keep true amplitude
    const scale = maxAmp > 1 ? maxAmp : 1;

    // Draw
    ctx.clearRect(0, 0, w, h);

    // Center line (horizontal)
    ctx.strokeStyle = "rgba(0,0,0,0.25)";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(0, h / 2);
    ctx.lineTo(w, h / 2);
    ctx.stroke();

    // Waveform
    ctx.strokeStyle = "rgba(0,0,0,0.75)";
    ctx.lineWidth = 2;
    ctx.lineJoin = "round";
    ctx.beginPath();

    const padding = 4;
    const drawH = h - padding * 2;

    // Draw from -π to +π so the wave crosses zero at the center
    for (let i = 0; i < samples; i++) {
      const x = i;
      const t = (i / samples - 0.5) * 8 * Math.PI; // -π to +π
      let sample = 0;
      for (const { gain, ratio } of harmonicData) {
        if (gain > 0 && ratio > 0) {
          sample += gain * Math.sin(t * ratio);
        }
      }
      sample /= scale;
      const y = padding + (drawH / 2) * (1 - sample);
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();
  }, [harmonicData]);

  return (
    <div className="flex size-full overflow-hidden bg-[#D9D9D9]">
      <canvas ref={canvasRef} className="size-full" />
    </div>
  );
};

export default Oscilloscope;
