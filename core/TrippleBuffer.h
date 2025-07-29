#pragma once
#include <atomic>
#include <array>
#include <cstdint>

// A lock-free triple buffer for cross-thread handoff.
template<typename T>
class TripleBuffer
{
public:
    TripleBuffer()
    {
        active.store(0, std::memory_order_relaxed);
    }

    // Producer: Call from non-realtime thread to get the write slot.
    T& getWriteSlot() { return buffers[(active.load(std::memory_order_relaxed) + 1) % 3]; }

    // Producer: Call after writing to publish the new buffer.
    void publish() { active.store((active.load(std::memory_order_relaxed) + 1) % 3, std::memory_order_acq_rel); }

    // Consumer: Call from the realtime/audio thread to read the current buffer.
    const T& getReadSlot() const { return buffers[active.load(std::memory_order_acquire)]; }

private:
    std::array<T, 3> buffers;
    std::atomic<uint8_t> active;
};
