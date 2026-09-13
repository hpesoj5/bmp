#pragma once
#ifndef RING_H
#define RING_H

#include <array>
#include <atomic>
#include <cstddef>
#include <optional>
#include <type_traits>

// assumes only one thread for pushing, and one for popping

namespace bmp::core {

template <typename T, std::size_t Size>
requires std::is_default_constructible_v<T>
class RingBuffer {
public:
    bool push(const T&);
    bool push(T&&)
    requires std::move_constructible<T>;

    std::optional<T> pop();
    std::size_t size() const;
    constexpr size_t capacity() const;

private:
    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> readIdx {};
    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> writeIdx {};
    std::array<T, Size + 1> arr;
};

template <typename T, std::size_t Size>
requires std::is_default_constructible_v<T>
bool RingBuffer<T, Size>::push(const T& val) {
    if (size() == Size) return false;
    std::size_t write { writeIdx.load(std::memory_order_relaxed) };

    arr[write] = val;

    writeIdx.store((write + 1) % (Size + 1), std::memory_order_release);
    return true;
}

template <typename T, std::size_t Size>
requires std::is_default_constructible_v<T>
bool RingBuffer<T, Size>::push(T&& val)
requires std::move_constructible<T>
{
    if (size() == Size) return false;
    std::size_t write { writeIdx.load(std::memory_order_relaxed) };

    arr[write] = std::move(val);

    writeIdx.store((write + 1) % (Size + 1), std::memory_order_release);
    return true;
}

template <typename T, std::size_t Size>
requires std::is_default_constructible_v<T>
std::optional<T> RingBuffer<T, Size>::pop() {
    std::size_t read { readIdx.load(std::memory_order_relaxed) };
    if (read == writeIdx.load(std::memory_order_acquire)) return std::nullopt;
    std::optional<T> result;

    if constexpr (std::move_constructible<T>) result = std::move(arr[read]);
    else result = arr[read];

    readIdx.store((read + 1) % (Size + 1), std::memory_order_release);

    return result;
}

template <typename T, std::size_t Size>
requires std::is_default_constructible_v<T>
std::size_t RingBuffer<T, Size>::size() const {
    std::size_t read { readIdx.load(std::memory_order_acquire) };
    std::size_t write { writeIdx.load(std::memory_order_acquire) };

    if (write >= read) return write - read;
    else return Size + 1 - (read - write);
}

template <typename T, std::size_t Size>
requires std::is_default_constructible_v<T>
constexpr std::size_t RingBuffer<T, Size>::capacity() const {
    return Size;
}

}  // namespace bmp::core

#endif
