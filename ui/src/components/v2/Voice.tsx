import NumberInput from "./Input";
import Envelope from "./Envelope";

interface VoiceProps {
  voiceId: string;
  voiceName: string;
}

const Voice = ({ voiceId, voiceName }: VoiceProps) => {
  return (
    <div className="flex relative flex-col gap-1">
      <h1 className="absolute top-0 left-1 text-[12px]">{voiceName}</h1>
      <Envelope voiceId={voiceId} />
      <div className="flex gap-1">
        <div className="grid grid-cols-3 gap-1 w-full">
          <NumberInput
            id={`${voiceId}_ratio`}
            label="R"
            min={0}
            max={12}
            step={0.01}
          />
          <NumberInput
            id={`${voiceId}_attack`}
            label="A"
            min={0}
            max={100}
            step={1}
          />
          <NumberInput
            id={`${voiceId}_sustain`}
            label="S"
            min={0}
            max={100}
            step={1}
          />
          <NumberInput
            id={`${voiceId}_gain`}
            label="G"
            min={0}
            max={1}
            step={0.01}
          />
          <NumberInput
            id={`${voiceId}_decay`}
            label="D"
            min={0}
            max={100}
            step={1}
          />
          <NumberInput
            id={`${voiceId}_release`}
            label="R"
            min={0}
            max={100}
            step={1}
          />
        </div>
      </div>
    </div>
  );
};

export default Voice;
