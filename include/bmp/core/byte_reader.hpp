#pragma once
#ifndef BYTE_READER_H
#define BYTE_READER_H

#include <concepts>
#include <cstddef>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace bmp::core {

using BufferView = std::span<const std::byte>;

template <typename T>
std::optional<T> read_bytes(BufferView buf, std::size_t offset, std::size_t length);

template <typename T>
requires std::integral<T>
std::optional<T> read_bytes(BufferView buf, std::size_t offset, std::size_t length) {
    if (length > sizeof(T)) return std::nullopt;
    if (offset > buf.size() || length > buf.size() - offset) return std::nullopt;
    T value {};
    std::memcpy(reinterpret_cast<std::byte*>(&value) + (sizeof(T) - length), buf.data() + offset, length);
    if constexpr (std::endian::native == std::endian::little) value = std::byteswap(value);
    return value;
}

template <>
inline std::optional<std::string> read_bytes<std::string>(BufferView buf, std::size_t offset, std::size_t length) {
    if (offset > buf.size() || length > buf.size() - offset) return std::nullopt;
    std::string_view raw(reinterpret_cast<const char*>(buf.data() + offset), length);
    auto last { raw.find_last_not_of(' ') };
    std::string_view res { raw.substr(0, last + 1) };
    auto end { res.find_last_not_of(' ') };
    if (end != std::string::npos) res = res.substr(0, end + 1);
    return std::string(res);
}

}  // namespace bmp::core

#endif
