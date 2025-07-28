#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "ModRoute.h"

class ModSource
{
public:
    virtual ~ModSource() = default;
    virtual void prepare(double sampleRate) = 0;
    virtual void process(int numSamples) = 0;
    virtual float getValue() const = 0;         // Range -1..+1
    virtual ModSourceType getType() const = 0;  // e.g. LFO1, Macro1, etc.
};

// Example LFO source:
class LfoSource : public ModSource
{
public:
    LfoSource(ModSourceType type, float freqHz = 1.0f)
        : type(type), freq(freqHz), phase(0.0f), sr(44100.0) {}

    void prepare(double sampleRate) override { sr = sampleRate; phase = 0.0f; }
    void process(int numSamples) override
    {
        float phaseInc = juce::MathConstants<float>::twoPi * freq / float(sr);
        phase += phaseInc * numSamples;
        if (phase > juce::MathConstants<float>::twoPi) phase -= juce::MathConstants<float>::twoPi;
        lastValue = std::sin(phase);
    }
    float getValue() const override { return lastValue; }
    ModSourceType getType() const override { return type; }
    void setFrequency(float f) { freq = f; }
private:
    ModSourceType type;
    float freq, phase, sr, lastValue = 0.0f;
};

// Example Macro source (UI knob):
class MacroSource : public ModSource
{
public:
    MacroSource(ModSourceType type) : type(type) {}
    void prepare(double) override {}
    void process(int) override {}
    float getValue() const override { return value; }
    ModSourceType getType() const override { return type; }
    void setValue(float v) { value = v; }
private:
    ModSourceType type;
    float value = 0.0f;
};
