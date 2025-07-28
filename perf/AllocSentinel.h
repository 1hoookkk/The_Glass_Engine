#pragma once

// ============================================================================
// AllocSentinel.h
// Provides a mechanism to track memory allocations, primarily to detect
// unexpected allocations in performance-critical code sections.
// ============================================================================

#include <cstdlib> // For size_t, malloc, free
#include <atomic>

namespace detail
{
    // These functions are implemented in AllocSentinel.cpp to override
    // global new/delete for this compilation unit.
    void* custom_new_impl(size_t size);
    void custom_delete_impl(void* ptr) noexcept;

    // Use a flag to enable/disable tracking, and an atomic counter
    extern std::atomic<bool> g_isTrackingAllocations;
    extern std::atomic<long long> g_allocationCount;
}

// Global new/delete overloads that call our custom implementations
// These will only apply to code compiled with AllocSentinel.cpp, due to symbol visibility
void* operator new(size_t size);
void operator delete(void* ptr) noexcept;
void* operator new[](size_t size);
void operator delete[](void* ptr) noexcept;

// ============================================================================
// Public API for AllocSentinel
// ============================================================================
class AllocSentinel
{
public:
    AllocSentinel()
    {
        detail::g_allocationCount = 0;
        detail::g_isTrackingAllocations = true;
    }

    ~AllocSentinel()
    {
        detail::g_isTrackingAllocations = false;
    }

    long long getAllocationCount() const
    {
        return detail::g_allocationCount.load();
    }
};