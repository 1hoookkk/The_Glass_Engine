#pragma once
#include <atomic>

struct Grain {
    std::atomic<bool> active{false};
    float startPos = 0;
    float position = 0;
    float duration = 0;
    float age = 0;
    float pitch = 1.0f; // <-- make sure you have this!
    float amplitude = 1.0f;
    float pan = 0;      // -1..1
    void reset() {
        active.store(false, std::memory_order_release);
        startPos = position = age = duration = 0;
        pitch = 1.0f;
        amplitude = 1.0f;
        pan = 0;
    }
};
