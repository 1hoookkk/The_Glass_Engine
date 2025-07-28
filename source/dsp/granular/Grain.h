// source/dsp/granular/Grain.h
#pragma once
#include <atomic>
#include <cmath>

struct Grain {
    // Core state
    std::atomic<bool> active{false};
    
    // Playback position
    float position = 0.0f;      // Current sample position in source buffer
    float startPos = 0.0f;      // Start position in source buffer
    float increment = 1.0f;     // Playback rate (1.0 = normal, 2.0 = octave up)
    
    // Timing
    float duration = 0.0f;      // Total grain duration in samples
    float age = 0.0f;           // Current age in samples
    
    // Amplitude
    float amplitude = 1.0f;     // Base amplitude
    float pan = 0.0f;           // Stereo pan (-1.0 to 1.0)
    
    // Window envelope
    float windowPhase = 0.0f;   // 0.0 to 1.0 through the window
    
    void reset() noexcept {
        active.store(false, std::memory_order_relaxed);
        position = 0.0f;
        age = 0.0f;
        windowPhase = 0.0f;
    }
    
    void spawn(float start, float dur, float pitch, float amp, float panPos) noexcept {
        startPos = start;
        position = start;
        duration = dur;
        increment = pitch;
        amplitude = amp;
        pan = panPos;
        age = 0.0f;
        windowPhase = 0.0f;
        active.store(true, std::memory_order_relaxed);
    }
    
    void tick() noexcept {
        age += 1.0f;
        position += increment;
        windowPhase = age / duration;
        if (windowPhase >= 1.0f) {
            reset(); // Deactivate grain when it finishes
        }
    }
};