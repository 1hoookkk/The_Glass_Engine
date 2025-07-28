// ============================================================================
// AllocSentinel.cpp
// Implementation of custom new/delete overloads for allocation tracking.
// ============================================================================

#include "AllocSentinel.h"
#include <cstdio> // For stderr, fprintf
#include <new> // For std::bad_alloc

// Global atomic variables defined in header
namespace detail
{
    std::atomic<bool> g_isTrackingAllocations { false };
    std::atomic<long long> g_allocationCount { 0 };
}

// Custom allocation function
void* detail::custom_new_impl(size_t size)
{
    if (detail::g_isTrackingAllocations.load())
    {
        detail::g_allocationCount++;
    }

    void* ptr = std::malloc(size);
    if (!ptr)
    {
        // If malloc fails, throw std::bad_alloc as per C++ standard
        throw std::bad_alloc();
    }
    return ptr;
}

// Custom deallocation function
void detail::custom_delete_impl(void* ptr) noexcept
{
    std::free(ptr);
}

// ============================================================================
// Global operator new/delete overloads
// These will be linked when AllocSentinel.cpp is part of the compilation unit
// ============================================================================

void* operator new(size_t size)
{
    return detail::custom_new_impl(size);
}

void operator delete(void* ptr) noexcept
{
    detail::custom_delete_impl(ptr);
}

void* operator new[](size_t size)
{
    return detail::custom_new_impl(size);
}

void operator delete[](void* ptr) noexcept
{
    detail::custom_delete_impl(ptr);
}