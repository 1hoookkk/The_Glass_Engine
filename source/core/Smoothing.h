// source/core/Smoothing.h
#pragma once
#include <cmath>
#include <algorithm>

class LinearSmoother {
public:
    LinearSmoother() = default;
        
    void reset(double sampleRate, float timeMs = 5.0f) noexcept {
        float samples = static_cast<float>(sampleRate * timeMs * 0.001);
        rampLength = std::max(1.0f, samples);
        rampIncrement = 1.0f / rampLength;
        // Ensure that current and target are synchronized on reset
        current = target;
        rampPosition = 1.0f;
    }
        
    void setTarget(float newTarget) noexcept {
        if (std::abs(newTarget - target) > 0.0001f) {
            target = newTarget;
            rampPosition = 0.0f; // Start ramp from current to new target
        }
    }
        
    float getNext() noexcept {
        if (rampPosition < 1.0f) {
            rampPosition = std::min(1.0f, rampPosition + rampIncrement);
            current = current + (target - current) * rampPosition; // Linear interpolation
        }
        return current;
    }
        
    void setImmediate(float value) noexcept {
        current = target = value;
        rampPosition = 1.0f;
    }

private:
    float current = 0.0f;
    float target = 0.0f;
    float rampPosition = 1.0f; // 0.0 = start of ramp, 1.0 = end of ramp
    float rampLength = 1.0f;    // In samples
    float rampIncrement = 1.0f; // Amount to increment rampPosition per sample
};