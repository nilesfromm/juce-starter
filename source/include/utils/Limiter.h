#pragma once

#include <cmath>
#include <juce_core/juce_core.h>

inline void limiter (float* buffer, int sampleCount)
{
    if (buffer == nullptr)
    {
        return;
    }
    bool firstWarning = true;
    for (int i = 0; i < sampleCount; i++)
    {
        float x = buffer[i];
        bool silence = false;
        if (isnan (x))
        {
            DBG ("Warning: NaN value in buffer");
            silence = true;
        }
        else if (isinf (x))
        {
            DBG ("Warning: Inf value in buffer");
            silence = true;
        }
        else if (x < -2.0f || x > 2.0f)
        {
            DBG ("Warning: Value out of range in buffer" + std::to_string (x));
            silence = true;
        }
        else if (x < -1.0f)
        {
            if (firstWarning)
            {
                DBG ("Warning: Value out of range in buffer");
                firstWarning = false;
            }
            buffer[i] = -1.0f;
        }
        else if (x > 1.0f)
        {
            if (firstWarning)
            {
                DBG ("Warning: Value out of range in buffer");
                firstWarning = false;
            }
            buffer[i] = 1.0f;
        }
        if (silence)
        {
            memset (buffer, 0, sampleCount * sizeof (float));
            return;
        }
    }
}