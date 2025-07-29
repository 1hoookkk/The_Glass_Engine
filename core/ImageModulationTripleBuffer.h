#pragma once

#include <array>
#include <atomic>
#include <cstddef>

struct GLsync;

class ImageModulationTripleBuffer
{
public:
    static constexpr int NumBuffers = 3;

    struct Buffer {
        float* data = nullptr;
        size_t size = 0;
        std::atomic<bool> ready { false };
        GLsync* sync = nullptr;
    };

    ImageModulationTripleBuffer(size_t bufferSize);
    ~ImageModulationTripleBuffer();

    void beginWrite(int bufferIndex);
    void endWrite(int bufferIndex);
    void pollAndMarkComplete();

    float* getWritePointer(int bufferIndex);
    const float* getBufferData(int bufferIndex) const;

    int getLatestAvailableBufferIndex() const;
    int getNextWriteBufferIndex();
    size_t getBufferSize() const { return buffers[0].size; }

private:
    void allocateBuffers(size_t bufferSize);
    void freeBuffers();

    std::array<Buffer, NumBuffers> buffers;
    std::atomic<int> latestReadyBuffer { -1 };
    int writeBufferIndex = 0;
};
