#pragma once
#include <atomic>
#include <array>

template<typename T>
class TripleBuffer {
public:
    TripleBuffer() = default;
    
    // GPU thread writes
    void write(const T& data) noexcept {
        buffers[writeIdx] = data;
        readyIdx.store(writeIdx, std::memory_order_release);
        writeIdx = freeIdx.exchange(writeIdx, std::memory_order_acq_rel);
    }
    
    // Audio thread reads
    bool read(T& data) noexcept {
        const int ready = readyIdx.exchange(-1, std::memory_order_acquire);
        if (ready >= 0) {
            data = buffers[ready];
            freeIdx.store(readIdx, std::memory_order_release);
            readIdx = ready;
            return true;
        }
        return false;
    }

private:
    std::array<T, 3> buffers{};
    std::atomic<int> readyIdx{-1};
    std::atomic<int> freeIdx{1};
    int writeIdx{0};
    int readIdx{2};
};
