// source/core/RealtimeConfig.h
#pragma once
#include <cassert>

#ifdef DEBUG
    #define RT_ASSERT(x) assert(x)
    // RT_NOALLOC_CHECK() is a placeholder for a more robust check in non-debug builds
    #define RT_NOALLOC_CHECK() static int dummy = (new int, 0); (void)dummy;
#else
    #define RT_ASSERT(x) ((void)0)
    #define RT_NOALLOC_CHECK() ((void)0)
#endif

// M0 specific constants
constexpr size_t GRAIN_POOL_SIZE = 512;
constexpr size_t WINDOW_TABLE_SIZE = 4096;
constexpr float SMOOTHING_TIME_MS = 5.0f;