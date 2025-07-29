#pragma once
#include <vector>
#include <cstdint>

// Structure to hold all modulation data published by the GPU/UI.
struct ModulationBuffer
{
    std::vector<float> brightness;        // Example: one brightness value per "scanline" or region.
    std::vector<float> probabilityField;  // Example: probability for grain spawn, size matches grid or image size.
    uint64_t           timestampMs = 0;   // Timestamp for monitoring staleness, in ms.

    void resize(size_t size)
    {
        brightness.resize(size, 0.0f);
        probabilityField.resize(size, 0.0f);
    }
};
