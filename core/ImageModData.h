#pragma once
#include <array>
#include <cstdint>

struct ImageModData {
    float brightness{0.0f};
    float centerR{0.0f};
    float centerG{0.0f};
    float centerB{0.0f};
    std::array<float, 16> densityCurve{};
    uint64_t frameNumber{0};

    void reset() noexcept {
        brightness = 0.0f;
        centerR = centerG = centerB = 0.0f;
        densityCurve.fill(0.0f);
        frameNumber = 0;
    }
};
