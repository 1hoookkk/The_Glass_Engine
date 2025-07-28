#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class ParamMirror
{
public:
    ParamMirror(float initialValue = 0.0f)
        : baseValue(initialValue), modSum(0.0f), smoothedValue(initialValue) {}

    void setBase(float v)               { baseValue = v; }
    void addMod(float v)                { modSum += v; }
    void finalize()
    {
        smoothedValue.setTargetValue(baseValue + modSum);
        modSum = 0.0f;
    }
    void reset(double sampleRate)
    {
        smoothedValue.reset(sampleRate, 0.01);
        smoothedValue.setCurrentAndTargetValue(baseValue);
    }
    void tick()                         { smoothedValue.skip(1); }
    float get() const                   { return smoothedValue.getCurrentValue(); }
private:
    float baseValue, modSum;
    juce::SmoothedValue<float> smoothedValue;
};