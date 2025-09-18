interface EnvelopeProps {
  attack: number;
  decay: number;
  sustain: number;
  release: number;
}

const Envelope = ({ attack, decay, sustain, release }: EnvelopeProps) => {
  const p1x = attack * 40;
  const p1y = 100;
  const p2x = (attack + decay) * 40;
  const p2y = sustain * 100;
  const p3x = 80;
  const p3y = sustain * 100;
  const p4x = 80 + release * 40;
  const p4y = 0;

  return (
    <div className="flex w-[252px] h-[52px] bg-[#D9D9D9]">
      <svg
        width="100%"
        height="100%"
        viewBox="0 0 120 120"
        preserveAspectRatio="none"
      >
        <path
          d={`M 0 120 L 
            ${p1x} ${120 - p1y} L 
            ${p2x} ${120 - p2y} L 
            ${p3x} ${120 - p3y} L 
            ${p4x} ${120 - p4y}`}
          fill="none"
          stroke="#333333"
          strokeWidth="1"
          vector-effect="non-scaling-stroke"
        />
      </svg>
    </div>
  );
};

export default Envelope;
