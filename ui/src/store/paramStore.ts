import { create } from "zustand";
import * as Juce from "../juce/index.js";

const updateParam = Juce.getNativeFunction("updateParam");
const loadState = Juce.getNativeFunction("loadState") as () => Promise<{
  schema: ParamDef[];
  values: Record<string, number>;
}>;

export interface ParamDef {
  id: string;
  name: string;
  min: number;
  max: number;
  default: number;
  step: number;
  skew: number;
}

interface ParamStore {
  values: Record<string, number>;
  schema: ParamDef[];
  setParam: (id: string, value: number) => void;
  _setFromBackend: (id: string, value: number) => void;
  _bulkSetFromBackend: (values: Record<string, number>) => void;
  hydrate: () => Promise<void>;
}

export const useParamStore = create<ParamStore>((set) => ({
  values: {},
  schema: [],

  setParam: (id, value) => {
    set((s) => ({ values: { ...s.values, [id]: value } }));
    updateParam(id, value);
  },

  _setFromBackend: (id, value) => {
    set((s) => ({ values: { ...s.values, [id]: value } }));
  },

  _bulkSetFromBackend: (values) => {
    set((s) => ({ values: { ...s.values, ...values } }));
  },

  hydrate: async () => {
    const state = await loadState();
    set({ schema: state.schema, values: state.values });
  },
}));
