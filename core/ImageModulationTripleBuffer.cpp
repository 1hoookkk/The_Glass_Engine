#include "ImageModulationTripleBuffer.h"
#include <GL/gl.h>
#include <cstdlib>

ImageModulationTripleBuffer::ImageModulationTripleBuffer(size_t bufferSize)
{
    allocateBuffers(bufferSize);
}

ImageModulationTripleBuffer::~ImageModulationTripleBuffer()
{
    freeBuffers();
}

void ImageModulationTripleBuffer::allocateBuffers(size_t bufferSize)
{
    for (auto& buf : buffers)
    {
        buf.data = static_cast<float*>(std::malloc(bufferSize * sizeof(float)));
        buf.size = bufferSize;
        buf.ready.store(false);
        buf.sync = nullptr;
    }
}

void ImageModulationTripleBuffer::freeBuffers()
{
    for (auto& buf : buffers)
    {
        if (buf.data) std::free(buf.data);
        if (buf.sync) glDeleteSync(buf.sync);

        buf.data = nullptr;
        buf.sync = nullptr;
    }
}

void ImageModulationTripleBuffer::beginWrite(int bufferIndex)
{
    if (buffers[bufferIndex].sync)
    {
        glDeleteSync(buffers[bufferIndex].sync);
        buffers[bufferIndex].sync = nullptr;
    }
    buffers[bufferIndex].ready.store(false, std::memory_order_release);
}

void ImageModulationTripleBuffer::endWrite(int bufferIndex)
{
    buffers[bufferIndex].sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void ImageModulationTripleBuffer::pollAndMarkComplete()
{
    for (int i = 0; i < NumBuffers; ++i)
    {
        auto& buf = buffers[i];
        if (!buf.ready.load(std::memory_order_acquire) && buf.sync)
        {
            GLenum status = glClientWaitSync(buf.sync, 0, 0);
            if (status == GL_ALREADY_SIGNALED || status == GL_CONDITION_SATISFIED)
            {
                buf.ready.store(true, std::memory_order_release);
                latestReadyBuffer.store(i, std::memory_order_release);
                glDeleteSync(buf.sync);
                buf.sync = nullptr;
            }
        }
    }
}

int ImageModulationTripleBuffer::getNextWriteBufferIndex()
{
    writeBufferIndex = (writeBufferIndex + 1) % NumBuffers;
    return writeBufferIndex;
}

float* ImageModulationTripleBuffer::getWritePointer(int bufferIndex)
{
    return buffers[bufferIndex].data;
}

const float* ImageModulationTripleBuffer::getBufferData(int bufferIndex) const
{
    return buffers[bufferIndex].data;
}

int ImageModulationTripleBuffer::getLatestAvailableBufferIndex() const
{
    return latestReadyBuffer.load(std::memory_order_acquire);
}
