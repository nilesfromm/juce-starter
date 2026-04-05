import { useParamStore } from "./paramStore";

declare global {
  interface Window {
    __JUCE__: {
      backend: {
        addEventListener: (
          event: string,
          callback: (data: any) => void,
        ) => void;
        removeEventListener: (token: any) => void;
        emitEvent: (event: string, data: any) => void;
      };
      initialisationData: Record<string, any>;
    };
  }
}

// Listen for individual parameter changes (DAW automation)
window.__JUCE__.backend.addEventListener(
  "paramChanged",
  (event: { id: string; value: number }) => {
    useParamStore.getState()._setFromBackend(event.id, event.value);
  },
);

// Listen for bulk state loads (preset changes)
window.__JUCE__.backend.addEventListener(
  "stateLoaded",
  (event: Record<string, number>) => {
    useParamStore.getState()._bulkSetFromBackend(event);
  },
);
