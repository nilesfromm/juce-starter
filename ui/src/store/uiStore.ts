import { create } from "zustand";

interface UIStore {
  selectedVoice: string;
  setSelectedVoice: (id: string) => void;
}

export const useUIStore = create<UIStore>((set) => ({
  selectedVoice: "h1",
  setSelectedVoice: (id) => set({ selectedVoice: id }),
}));
