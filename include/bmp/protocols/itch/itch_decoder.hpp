#pragma once
#ifndef ITCH_DECODER_H
#define ITCH_DECODER_H

#include "bmp/core/decoder_interface.hpp"
#include "bmp/core/message_types.hpp"

using namespace bmp::core;

namespace bmp::itch {

struct ItchResult {
    DecodeStatus status;
    std::optional<ItchMessage> message;
};

class ItchDecoder : public IMessageDecoder<ItchDecoder> {
public:
    ItchResult decode_impl(BufferView buf) const;
};

}  // namespace bmp::itch

#endif
