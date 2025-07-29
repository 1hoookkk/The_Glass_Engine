#pragma once
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class WindowType { Hann, Hamming, Blackman };

inline std::vector<float> makeWindow(size_t n, WindowType type = WindowType::Hann)
{
    std::vector<float> w(n);
    switch (type)
    {
        case WindowType::Hann:
            for (size_t i=0;i<n;++i)
                w[i] = 0.5f * (1.0f - std::cos(2.0 * M_PI * i / (n - 1)));
            break;
        case WindowType::Hamming:
            for (size_t i=0;i<n;++i)
                w[i] = 0.54f - 0.46f * std::cos(2.0 * M_PI * i / (n - 1));
            break;
        case WindowType::Blackman:
            for (size_t i=0;i<n;++i)
                w[i] = 0.42f - 0.5f * std::cos(2.0 * M_PI * i / (n - 1))
                                  + 0.08f * std::cos(4.0 * M_PI * i / (n - 1));
            break;
    }
    return w;
}