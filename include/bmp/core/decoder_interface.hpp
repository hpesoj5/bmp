#pragma once
#ifndef IDECODER_H
#define IDECODER_H

#include <cstddef>
#include <span>

using BufferView = std::span<const std::byte>;

enum DecodeStatus {
    Ok,
    UnknownType,
    Truncated,
    ParseError,
};

template <typename Derived>
class IMessageDecoder {
public:
    auto decode(BufferView buf) const { return static_cast<const Derived*>(this)->decode_impl(buf); }
};

#endif
