#pragma once

#include <array>

struct ParamDef
{
    const char* id;
    const char* name;
    float min;
    float max;
    float defaultVal;
    float step;
    float skew;
};

// id, name, min, max, default, step, skew
constexpr ParamDef PARAM_SCHEMA[] = {
    // H1
    { "h1_ratio", "ratio", 0.f, 12.f, 0.5f, 0.0001f, 1.f },
    { "h1_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h1_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h1_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h1_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h1_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H2
    { "h2_ratio", "ratio", 0.f, 12.f, 1.0f, 0.0001f, 1.f },
    { "h2_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h2_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h2_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h2_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h2_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H3
    { "h3_ratio", "ratio", 0.f, 12.f, 2.0f, 0.0001f, 1.f },
    { "h3_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h3_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h3_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h3_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h3_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H4
    { "h4_ratio", "ratio", 0.f, 12.f, 3.0f, 0.0001f, 1.f },
    { "h4_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h4_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h4_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h4_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h4_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H5
    { "h5_ratio", "ratio", 0.f, 12.f, 4.0f, 0.0001f, 1.f },
    { "h5_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h5_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h5_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h5_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h5_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H6
    { "h6_ratio", "ratio", 0.f, 12.f, 5.0f, 0.0001f, 1.f },
    { "h6_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h6_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h6_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h6_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h6_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H7
    { "h7_ratio", "ratio", 0.f, 12.f, 6.0f, 0.0001f, 1.f },
    { "h7_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h7_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h7_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h7_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h7_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H8
    { "h8_ratio", "ratio", 0.f, 12.f, 7.0f, 0.0001f, 1.f },
    { "h8_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h8_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h8_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h8_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h8_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // H9
    { "h9_ratio", "ratio", 0.f, 12.f, 8.0f, 0.0001f, 1.f },
    { "h9_gain", "gain", 0.f, 1.f, 0.25f, 0.0001f, 1.f },
    { "h9_attack", "attack", 0.f, 100.f, 2.f, 1.f, 1.f },
    { "h9_decay", "decay", 0.f, 100.f, 30.f, 1.f, 1.f },
    { "h9_sustain", "sustain", 0.f, 100.f, 0.f, 1.f, 1.f },
    { "h9_release", "release", 0.f, 100.f, 25.f, 1.f, 1.f },

    // Global
    { "noise", "noise", 0.f, 1.f, 0.f, 0.01f, 1.f },
};

constexpr int TOTAL_PARAMS = sizeof (PARAM_SCHEMA) / sizeof (PARAM_SCHEMA[0]);
