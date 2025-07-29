// source/dsp/granular/WindowTable.h
#pragma once
#include <array>
#include <cmath>

#ifndef M_PI // Define M_PI if not already defined (common in Visual Studio)
#define M_PI 3.14159265358979323846
#endif

template<size_t TABLE_SIZE = 4096>
class WindowTable {
public:
    WindowTable() {
        for (size_t i = 0; i < TABLE_SIZE; ++i) {
            float phase = static_cast<float>(i) / (TABLE_SIZE - 1);
            table[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * phase));
        }
    }

    float lookup(float phase) const noexcept {
        phase = std::clamp(phase, 0.0f, 1.0f);
        float index = phase * (TABLE_SIZE - 1);
        size_t i0 = static_cast<size_t>(index);
        size_t i1 = std::min(i0 + 1, TABLE_SIZE - 1);
        float frac = index - i0;
        return table[i0] + frac * (table[i1] - table[i0]);
    }

private:
    std::array<float, TABLE_SIZE> table;
};

// Global instance of the WindowTable for easy access
inline const WindowTable<> g_windowTable;