// source/dsp/granular/GrainPool.h
#pragma once
#include <array>
#include <atomic>
#include "Grain.h" // Add this line to include the Grain struct definition

template<size_t POOL_SIZE = 512>
class GrainPool {
public:
    GrainPool() = default;

    Grain* allocate() noexcept {
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            size_t idx = (nextIndex + i) % POOL_SIZE;
            bool expected = false;
            if (grains[idx].active.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
                nextIndex = (idx + 1) % POOL_SIZE;
                return &grains[idx];
            }
        }
        return nullptr;
    }

    void reset() noexcept {
        for (auto& grain : grains) {
            grain.reset();
        }
        nextIndex = 0;
    }

    // Iterator support for range-based for loops
    Grain* begin() { return grains.data(); }
    Grain* end()   { return grains.data() + POOL_SIZE; }

    const Grain* begin() const { return grains.data(); }
    const Grain* end()   const { return grains.data() + POOL_SIZE; }

    size_t activeCount() const noexcept {
        size_t count = 0;
        for (const auto& grain : grains) {
            if (grain.active.load(std::memory_order_relaxed)) {
                count++;
            }
        }
        return count;
    }

private:
    std::array<Grain, POOL_SIZE> grains;
    std::atomic<size_t> nextIndex{0}; // For allocation
};