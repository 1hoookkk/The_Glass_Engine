// source/core/LockFreeFIFO.h
#pragma once
#include <atomic>
#include <array>

template <typename T, size_t SIZE>
class LockFreeFIFO
{
    static_assert((SIZE & (SIZE - 1)) == 0, "SIZE must be power of 2");
    
public:
    LockFreeFIFO() : head(0), tail(0) {}
    
    // Producer: try to push an item
    bool push(const T& item)
    {
        const size_t currentTail = tail.load(std::memory_order_relaxed);
        const size_t nextTail = (currentTail + 1) & MASK;
        
        if (nextTail == head.load(std::memory_order_acquire))
            return false; // Queue is full
            
        buffer[currentTail] = item;
        tail.store(nextTail, std::memory_order_release);
        return true;
    }
    
    // Producer: try to push by moving
    bool push(T&& item)
    {
        const size_t currentTail = tail.load(std::memory_order_relaxed);
        const size_t nextTail = (currentTail + 1) & MASK;
        
        if (nextTail == head.load(std::memory_order_acquire))
            return false; // Queue is full
            
        buffer[currentTail] = std::move(item);
        tail.store(nextTail, std::memory_order_release);
        return true;
    }
    
    // Consumer: try to pop an item
    bool pop(T& item)
    {
        const size_t currentHead = head.load(std::memory_order_relaxed);
        
        if (currentHead == tail.load(std::memory_order_acquire))
            return false; // Queue is empty
            
        item = buffer[currentHead];
        head.store((currentHead + 1) & MASK, std::memory_order_release);
        return true;
    }
    
    // Check if empty (may be stale immediately)
    bool empty() const
    {
        return head.load(std::memory_order_acquire) == 
               tail.load(std::memory_order_acquire);
    }
    
    // Check if full (may be stale immediately)  
    bool full() const
    {
        const size_t currentTail = tail.load(std::memory_order_acquire);
        const size_t nextTail = (currentTail + 1) & MASK;
        return nextTail == head.load(std::memory_order_acquire);
    }
    
    // Get approximate size (may be stale)
    size_t size() const
    {
        const size_t currentHead = head.load(std::memory_order_acquire);
        const size_t currentTail = tail.load(std::memory_order_acquire);
        return (currentTail - currentHead) & MASK;
    }
    
    // Clear all items (not thread-safe with concurrent producers/consumers)
    void clear()
    {
        head.store(0, std::memory_order_release);
        tail.store(0, std::memory_order_release);
    }

private:
    static constexpr size_t MASK = SIZE - 1;
    
    std::array<T, SIZE> buffer;
    alignas(64) std::atomic<size_t> head; // Cache line alignment
    alignas(64) std::atomic<size_t> tail;
};