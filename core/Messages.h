// source/core/Messages.h
#pragma once
#include <vector>
#include <cstdint>

struct SpectralMaskMsg
{
    uint32_t        id      = 0;
    std::vector<float> data;
    uint32_t        width   = 0;
    uint32_t        height  = 0;
};
