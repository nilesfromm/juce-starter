import { useEffect, useState } from "react";
import "./App.css";
import SpectrogramRow from "./components/v3/SpectrogramRow";
import Oscilloscope from "./components/v3/Oscilloscope";
import "./store/bridge";
import { useParamStore } from "./store/paramStore";

const HARMONICS = [
  { id: "h8", label: "H8" },
  { id: "h7", label: "H7" },
  { id: "h6", label: "H6" },
  { id: "h5", label: "H5" },
  { id: "h4", label: "H4" },
  { id: "h3", label: "H3" },
  { id: "h2", label: "H2" },
  { id: "h1", label: "H1" },
];

function App() {
  const [ready, setReady] = useState(false);
  const hydrate = useParamStore((s) => s.hydrate);

  useEffect(() => {
    hydrate().then(() => setReady(true));
  }, [hydrate]);

  if (!ready) return null;

  return (
    <div className="flex flex-col p-4 gap-3 bg-white" style={{ height: 500 }}>
      <div className="flex-1 flex flex-col gap-[2px] min-h-0 overflow-hidden">
        {/* Stage labels */}
        <div className="flex items-center gap-2 mb-1">
          <span className="w-6 shrink-0" />
          <div className="flex w-full text-[10px] text-[#666] select-none">
            <span className="flex-1 text-center">A</span>
            <span className="flex-1 text-center">D</span>
            <span className="flex-1 text-center">S</span>
            <span className="flex-1 text-center">R</span>
          </div>
        </div>

        {/* Spectrogram rows — H8 (top) to H1 (bottom) */}
        {HARMONICS.map(({ id, label }) => (
          <div key={id} className="flex-1 min-h-0">
            <SpectrogramRow id={id} label={label} />
          </div>
        ))}
      </div>

      {/* Oscilloscope */}
      <div className="h-[60px] shrink-0">
        <Oscilloscope />
      </div>
    </div>
  );
}

export default App;
