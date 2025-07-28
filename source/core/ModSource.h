// source/core/ModSource.h
#pragma once
#include <memory>
#include "ModRoute.h"

// Base class for modulation sources
struct ModSource {
    virtual ~ModSource() = default;
    virtual ModSourceType type() const = 0;
    virtual float nextValue() = 0;  // Get next modulation value [-1, 1]
    virtual void reset() {}
};

using ModSourcePtr = std::unique_ptr<ModSource>;

// Simple LFO implementation for testing
class SimpleLFO : public ModSource {
public:
    explicit SimpleLFO(ModSourceType t, float freq = 1.0f) 
        : sourceType(t), frequency(freq) {}
    
    ModSourceType type() const override { return sourceType; }
    
    float nextValue() override {
        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
        return std::sin(2.0f * M_PI * phase);
    }
    
    void reset() override { phase = 0.0f; }
    
    void setSampleRate(double sr) {
        sampleRate = sr;
        updateIncrement();
    }
    
    void setFrequency(float freq) {
        frequency = freq;
        updateIncrement();
    }

private:
    void updateIncrement() {
        if (sampleRate > 0) {
            phaseIncrement = frequency / static_cast<float>(sampleRate);
        }
    }
    
    ModSourceType sourceType;
    float frequency = 1.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    double sampleRate = 48000.0;
};