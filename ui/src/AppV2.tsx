import { useEffect, useState } from "react";
import "./App.css";
import Harmonics from "./components/v2/Harmonics";
import Envelope from "./components/v2/Envelope";
import Knob from "./components/v2/Knob";
import "./store/bridge";
import { useParamStore } from "./store/paramStore";
import { useUIStore } from "./store/uiStore";
import Oscilloscope from "./components/v2/Oscilloscope";

const EnvelopeKnob = ({ id, label }: { id: string; label: string }) => {
  const value = useParamStore((s) => s.values[id] ?? 0);
  return (
    <div className="flex flex-col items-center gap-1">
      <span className="text-[11px] text-[#666]">{label}</span>
      <Knob id={id} min={0} max={100} step={1} />
      <span className="text-[10px] text-[#999]">{value}ms</span>
    </div>
  );
};

const SelectedEnvelope = () => {
  const selectedVoice = useUIStore((s) => s.selectedVoice);

  return (
    <div className="flex flex-col gap-3 text-[#333333] bg-[#E8E8E8] rounded p-4">
      <Envelope voiceId={selectedVoice} />
      <div className="flex justify-around">
        <EnvelopeKnob id={`${selectedVoice}_attack`} label="Attack" />
        <EnvelopeKnob id={`${selectedVoice}_decay`} label="Decay" />
        <EnvelopeKnob id={`${selectedVoice}_sustain`} label="Sustain" />
        <EnvelopeKnob id={`${selectedVoice}_release`} label="Release" />
      </div>
    </div>
  );
};

function App() {
  const [ready, setReady] = useState(false);
  const hydrate = useParamStore((s) => s.hydrate);

  useEffect(() => {
    hydrate().then(() => setReady(true));
  }, [hydrate]);

  if (!ready) return null;

  return (
    <div className="flex flex-col size-full p-4 gap-4">
      <div className="grid grid-cols-3 gap-4 text-[#333333]">
        <div className="col-span-2">
          <Harmonics />
        </div>
        <div className="col-span-1 w-full h-[124px]">
          <Oscilloscope />
        </div>
      </div>
      <div className="text-[#333333]">
        <SelectedEnvelope />
      </div>
    </div>
  );
}

export default App;
