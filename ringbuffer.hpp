//
// Created by Lukas on 02.06.2026.
//

#ifndef ITCH_PARSER_RINGBUFFER_HPP
#define ITCH_PARSER_RINGBUFFER_HPP
#include <atomic>

template <typename T, const std::size_t Capacity>

class ringbuffer
{
private:
    T buffer [Capacity];
    alignas(64) std::atomic<std::size_t> producerIndex = 0;
    alignas(64) std::atomic<std::size_t> consumerIndex = 0;

public:
    ringbuffer()= default;
    bool push(const T& item)
    {
        std::size_t currentPI = producerIndex.load(std::memory_order_relaxed);
        std::size_t currentCI = consumerIndex.load(std::memory_order_acquire);

        if (currentPI - currentCI == Capacity) return false;

        buffer[currentPI % Capacity] = item;
        producerIndex.store(currentPI+1, std::memory_order_release);
        return true;
    }
    bool pop(T& itemSlot)
    {
        std::size_t currentPI = producerIndex.load(std::memory_order_acquire);
        std::size_t currentCI = consumerIndex.load(std::memory_order_relaxed);

        if (currentPI == currentCI) return false;
        itemSlot = buffer[currentCI % Capacity];
        consumerIndex.store(currentCI+1, std::memory_order_release);
        return true;
    }

    bool push_batch(const T* items, std::size_t count)
    {
        std::size_t currentPI = producerIndex.load(std::memory_order_relaxed);
        std::size_t currentCI = consumerIndex.load(std::memory_order_acquire);

        if (Capacity - (currentPI - currentCI) < count) return false;
        for (std::size_t i = 0; i < count; ++i)
        {
            buffer[(currentPI + i) % Capacity] = items[i];
        }
        producerIndex.store(currentPI + count, std::memory_order_release);
        return true;
    }

    std::size_t pop_batch(T* itemsSlot, std::size_t max_count)
    {
        std::size_t currentPI = producerIndex.load(std::memory_order_acquire);
        std::size_t currentCI = consumerIndex.load(std::memory_order_relaxed);

        std::size_t available = currentPI - currentCI;
        if (available == 0) return 0;

        std::size_t count = std::min(available, max_count);

        for (std::size_t i = 0; i < count; ++i)
        {
            itemsSlot[i] = buffer[(currentCI + i) % Capacity];
        }

        consumerIndex.store(currentCI + count, std::memory_order_release);
        return count;
    }
};

#endif //ITCH_PARSER_RINGBUFFER_HPP
