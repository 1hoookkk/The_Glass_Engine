// source/dsp/granular/GranularEngine.h
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <atomic>
#include <random>
#include "Grain.h"
#include "GrainPool.h"
#include "WindowTable.h"

class GranularEngine {
public:
    GranularEngine();
    ~GranularEngine() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    void process(juce::AudioBuffer<float>& buffer);
    
    // Source buffer management
    void setSourceBuffer(const juce::AudioBuffer<float>& source);
    
    // Parameters (these would normally come from your parameter system)
    void setGrainRate(float grainsPerSecond) { grainRate.store(grainsPerSecond); }
    void setGrainDuration(float ms) { grainDurationMs.store(ms); }
    void setPosition(float pos) { position.store(juce::jlimit(0.0f, 1.0f, pos)); }
    void setPositionSpread(float spread) { positionSpread.store(spread); }
    void setPitch(float pitchRatio) { pitch.store(pitchRatio); }
    void setPitchSpread(float spread) { pitchSpread.store(spread); }
    
    // Stats for performance monitoring
    size_t getActiveGrainCount() const { return grainPool.activeCount(); }

private:
    // Core components
    GrainPool<512> grainPool;
    juce::AudioBuffer<float> sourceBuffer;
    std::atomic<int> sourceLength{0};
    
    // Audio thread state
    double sampleRate = 48000.0;
    float samplesSinceLastGrain = 0.0f;
    std::mt19937 rng{42}; // Fixed seed for deterministic behavior
    std::uniform_real_distribution<float> random{-1.0f, 1.0f};
    
    // Parameters (atomics for thread safety)
    std::atomic<float> grainRate{20.0f};
    std::atomic<float> grainDurationMs{50.0f};
    std::atomic<float> position{0.5f};
    std::atomic<float> positionSpread{0.1f};
    std::atomic<float> pitch{1.0f};
    std::atomic<float> pitchSpread{0.0f};
    
    // Helper methods
    void spawnGrain();
    float getRandomSpread() { return random(rng); }
    
    // Pan calculation
    static void calculatePanGains(float pan, float& leftGain, float& rightGain) noexcept {
        const float piOver2 = juce::MathConstants<float>::halfPi;
        leftGain = std::cos((pan + 1.0f) * 0.25f * piOver2);
        rightGain = std::sin((pan + 1.0f) * 0.25f * piOver2);
    }
};
