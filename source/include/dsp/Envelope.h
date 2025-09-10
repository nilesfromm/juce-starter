#pragma once

const float SILENCE_THRESHOLD = 0.0001f;

class Envelope
{
public:
    float level;

    float attackMultiplier;
    float decayMultiplier;
    float sustainLevel;
    float releaseMultiplier;

    inline bool isActive() const
    {
        return level > SILENCE_THRESHOLD;
    }

    float reset()
    {
        level = 0.0f;
        target = 0.0f;
        multiplier = 0.0f;
    }

    float nextValue()
    {
        level = multiplier * (level - target) + target;

        if (level + target > 3.0f)
        {
            multiplier = decayMultiplier;
            target = sustainLevel;
        }

        return level;
    }

    inline bool isInAttack() const
    {
        return target >= 2.0f;
    }

    void attack()
    {
        level += SILENCE_THRESHOLD + SILENCE_THRESHOLD;
        target = 2.0f;
        multiplier = attackMultiplier;
    }

    void decay()
    {
        target = sustainLevel;
    }

    void release()
    {
        target = 0.0f;
        multiplier = releaseMultiplier;
    }

private:
    float target;
    float multiplier;
};