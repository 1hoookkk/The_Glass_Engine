#pragma once
#include <cstdint>

struct ImageModulationData {
    float brightness = 0.0f;
    float colorR = 0.0f;
    float colorG = 0.0f;
    float colorB = 0.0f;
    float positionX = 0.5f;
    float positionY = 0.5f;
    uint64_t frameNumber = 0;
    
    static constexpr size_t MAX_MODULATION_POINTS = 16;
    float modulationCurve[MAX_MODULATION_POINTS] = {};
};