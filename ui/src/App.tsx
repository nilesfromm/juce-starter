import { useEffect, useState } from "react";
import "./App.css";
import Harmonics from "./components/Harmonics";
import Voice from "./components/Voice";
import "./store/bridge";
import { useParamStore } from "./store/paramStore";

function App() {
  const [ready, setReady] = useState(false);
  const hydrate = useParamStore((s) => s.hydrate);

  useEffect(() => {
    hydrate().then(() => setReady(true));
  }, [hydrate]);

  if (!ready) return null;

  return (
    <div className="flex flex-col size-full p-4 gap-4">
      <div className="grid grid-cols-3 gap-4 items-center justify-center text-[#333333]">
        <Voice voiceId="h1" voiceName="H1" />
        <Voice voiceId="h2" voiceName="H2" />
        <Voice voiceId="h3" voiceName="H3" />
        <Voice voiceId="h4" voiceName="H4" />
        <Voice voiceId="h5" voiceName="H5" />
        <Voice voiceId="h6" voiceName="H6" />
        <Voice voiceId="h7" voiceName="H7" />
        <Voice voiceId="h8" voiceName="H8" />
        <Voice voiceId="h9" voiceName="N1" />
      </div>
      <div className="grid grid-cols-3 gap-4 items-center justify-center text-[#333333]">
        <div className="col-span-2">
          <Harmonics />
        </div>
        <div className="col-span-1 bg-blue-600 w-full h-[124px]"></div>
      </div>
    </div>
  );
}

export default App;
