interface ToggleProps {
  value: boolean;
  onChange: (value: boolean) => void;
}

const  Toggle = ({ value, onChange }: ToggleProps) => {
  return (
    <div className="flex text-[12px]">
      <button className={`size-6  flex items-center justify-center ${value ===false? "bg-[#333333] text-white": "bg-[#D9D9D9] text-black"}`} onClick={()=>onChange(false)}>
        <span className="">
          F
        </span>
      </button>
      <button className={`size-6 flex items-center justify-center ${value ===true? "bg-[#333333] text-white": "bg-[#D9D9D9] text-black"}`} onClick={()=>onChange(true)}>
        <span className="">
          R
        </span>
      </button>
    </div>
   );
}

export default Toggle;