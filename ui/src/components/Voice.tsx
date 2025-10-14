import { useState } from "react";
import NumberInput from "./Input";
import Toggle from "./Toggle";
import Envelope from "./Envelope";

interface VoiceProps {
  voiceId: string;
  voiceName: string;
}

const Voice = ({ voiceId, voiceName }: VoiceProps) => {
  const [attack, setAttack] = useState(0.0);
  const [decay, setDecay] = useState(0.0);
  const [sustain, setSustain] = useState(0.0);
  const [release, setRelease] = useState(0.0);

  const [isRatioLocked, setIsRatioLocked] = useState(false);

  return (
    <div className="flex relative flex-col gap-1">
      <h1 className="absolute top-0 left-1 text-[12px]">{voiceName}</h1>
      <Envelope
        attack={attack}
        decay={decay}
        sustain={sustain}
        release={release}
      />
      <div className="flex gap-1">
        {/* <div className="flex justify-between gap-1">
          <Toggle value={isRatioLocked} onChange={setIsRatioLocked} />
        </div> */}
        <div className="grid grid-cols-3 gap-1 w-full">
          {isRatioLocked ? (
            <NumberInput
              key={`${voiceId}-ratio`}
              id={`${voiceId}_frequency`}
              label="R"
              min={0}
              max={1}
              step={0.01}
            />
          ) : (
            <NumberInput
              key={`${voiceId}-frequency`}
              id={`${voiceId}_frequency`}
              label="F"
              min={0}
              max={1}
              step={0.01}
            />
          )}

          <NumberInput
            key={`${voiceId}-attack`}
            id={`${voiceId}_attack`}
            label="A"
            onChange={setAttack}
            min={0}
            max={1}
            step={0.01}
          />
          <NumberInput
            key={`${voiceId}-sustain`}
            id={`${voiceId}_sustain`}
            label="S"
            onChange={setSustain}
            min={0}
            max={1}
            step={0.01}
          />
          <NumberInput
            key={`${voiceId}-gain`}
            id={`${voiceId}_gain`}
            label="G"
            min={0}
            max={1}
            step={0.01}
          />
          <NumberInput
            key={`${voiceId}-decay`}
            id={`${voiceId}_decay`}
            label="D"
            onChange={setDecay}
            min={0}
            max={1}
            step={0.01}
          />
          <NumberInput
            key={`${voiceId}-release`}
            id={`${voiceId}_release`}
            label="R"
            onChange={setRelease}
            min={0}
            max={1}
            step={0.01}
          />
        </div>
      </div>
    </div>
  );
};

export default Voice;
