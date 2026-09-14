#pragma once
#ifndef FRAMING_H
#define FRAMING_H

#include "bmp/core/byte_reader.hpp"
#include "bmp/core/sequence_tracker.hpp"
#include "bmp/protocols/itch/itch_constants.hpp"
#include "bmp/protocols/itch/itch_decoder.hpp"

#include <cstddef>
#include <span>

namespace bmp::transport {

// uses itch decoder for now
using BufferView = std::span<const std::byte>;

template <typename Decoder>
void handle_packet(BufferView, Decoder&, SequenceTracker&);

template <>
inline void handle_packet<ItchDecoder>(BufferView packet, ItchDecoder& decoder, SequenceTracker& tracker) {
    auto session { read_bytes<std::string>(packet, itch::downstream::header::session.offset,
                                           itch::downstream::header::session.length) };
    auto seq_num { read_bytes<std::uint64_t>(packet, itch::downstream::header::sequence_number.offset,
                                             itch::downstream::header::sequence_number.length) };
    auto msg_count { read_bytes<std::uint16_t>(packet, itch::downstream::header::message_count.offset,
                                               itch::downstream::header::message_count.length) };

    if (!session || !seq_num || !msg_count) return;

    auto [skip, status] { tracker.register_sequence_number(session.value(), seq_num.value(), msg_count.value()) };

    if (status == SequenceStatus::Gap) {
        // do something
        return;
    }
    if (status == SequenceStatus::SessionMismatch) {
        // do something
        return;
    }
    if (status == SequenceStatus::Heartbeat) {
        // do something
        return;
    }
    if (status == SequenceStatus::Terminated) {
        // do something
        return;
    }

    auto messages { msg_count.value() };
    auto offset { itch::downstream::header::message_count.offset + itch::downstream::header::message_count.length };
    for (std::uint16_t i { 0 }; i < messages; ++i) {
        auto message_length { read_bytes<std::uint16_t>(packet, offset,
                                                        itch::downstream::header::messageLength_length) };
        if (!message_length) {
            // signal parse error
            return;
        }
        BufferView msg_data { packet.subspan(offset + itch::downstream::header::messageLength_length,
                                             message_length.value()) };
        if (i >= skip) decoder.decode(msg_data);

        offset += itch::downstream::header::messageLength_length + message_length.value();
    }
}

}  // namespace bmp::transport

#endif
