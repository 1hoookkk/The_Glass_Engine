#pragma once
#include "../core/ImageModData.h"
#include <thread>
#include <atomic>
#include <functional>

class GPUImageReader {
public:
    virtual ~GPUImageReader() = default;

    void start(std::function<void(const ImageModData&)> callback) {
        if (running) return;
        running = true;
        publishCallback = callback;
        workerThread = std::thread(&GPUImageReader::workerLoop, this);
    }

    void stop() {
        running = false;
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    virtual void setColorBoxFBO(unsigned int fbo, int w, int h) {
        framebuffer = fbo;
        width = w;
        height = h;
    }

protected:
    virtual void analyzeFrame(ImageModData& data) = 0;

    void workerLoop() {
        uint64_t frameCounter = 0;
        while (running) {
            ImageModData data;
            data.frameNumber = frameCounter++;
            analyzeFrame(data);
            if (publishCallback) publishCallback(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    std::atomic<bool> running{false};
    std::thread workerThread;
    std::function<void(const ImageModData&)> publishCallback;

    unsigned int framebuffer{0};
    int width{512};
    int height{512};
};
