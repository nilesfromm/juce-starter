import { useState } from "react";
import AppV1 from "./AppV1";
import AppV2 from "./AppV2";

const AppSelected = ({ version }: { version: "v1" | "v2" }) => {
  switch (version) {
    case "v1":
      return <AppV1 />;
    case "v2":
      return <AppV2 />;
    default:
      return <AppV1 />;
  }
};

const AppWrapper = () => {
  const [version, setVersion] = useState<"v1" | "v2">("v1");
  return (
    <div>
      <div className="flex w-full">
        <button
          className=" text-white p-0 w-full"
          style={{ backgroundColor: version === "v1" ? "#CCC" : "#DDD" }}
          onClick={() => setVersion("v1")}
        >
          V1
        </button>
        <button
          className=" text-white p-0 w-full"
          style={{ backgroundColor: version === "v2" ? "#CCC" : "#DDD" }}
          onClick={() => setVersion("v2")}
        >
          V2
        </button>
      </div>
      <AppSelected version={version} />
    </div>
  );
};

export default AppWrapper;
