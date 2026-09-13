#pragma once
#ifndef ITCH_DECODER_H
#define ITCH_DECODER_H

#include "../../core/decoder_interface.hpp"
#include "../../core/message_types.hpp"

using namespace bmp::core;

struct ItchResult {
    DecodeStatus status;
    std::optional<ItchMessage> message;
};

class ItchDecoder : public IMessageDecoder<ItchDecoder> {
public:
    ItchResult decode_impl(BufferView buf) const;
};

#endif
