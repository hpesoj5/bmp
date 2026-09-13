#pragma once
#ifndef MESSAGE_TYPE_H
#define MESSAGE_TYPE_H

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <variant>

using BufferView = std::span<const std::byte>;

namespace bmp::core {

enum class Side { Buy, Sell };

using Session        = std::string;
using SequenceNumber = std::uint64_t;
using MessageCount   = std::uint16_t;
using Length         = std::uint16_t;

struct DownstreamHeader {
    Session session;
    SequenceNumber sequence_number;
    MessageCount message_count;
};  // followed by message_count of DowstreamMessage

struct DowstreamMessage {
    std::uint16_t length;
    BufferView data;
};

struct RequestPacket {
    Session session;
    SequenceNumber first_sequence_number;
    MessageCount message_count;
};

using MessageType     = char;
using StockLocate     = std::uint16_t;
using TrackingNumber  = std::uint16_t;
using nsSinceMidnight = std::chrono::nanoseconds;
using OrderRefNumber  = std::uint64_t;
using Quantity        = std::uint32_t;
using Price           = std::uint32_t;
using MatchNumber     = std::uint64_t;

struct EventHeader {
    MessageType message_type;
    StockLocate stock_locate;
    TrackingNumber tracking_number;
    nsSinceMidnight timestamp;
};

struct Order {
    EventHeader header;
    OrderRefNumber order_ref;
    Side side;  // 'B' or 'S' in raw data
    Quantity shares;
    std::string symbol;
    Price price;  // fixed point, precision 4 (4 d.p.)
    std::string attributor { "NSDQ" };
};

struct OrderExecuted {
    EventHeader header;
    OrderRefNumber order_ref;
    Quantity executed_shares;
    MatchNumber match_number;
};

struct OrderExecutedWithPrice {
    EventHeader header;
    OrderRefNumber order_ref;
    Quantity executed_shares;
    MatchNumber match_number;
    char printable;
    Price price;  // 4 d.p.
};

struct OrderCancelled {
    EventHeader header;
    OrderRefNumber order_ref;
    Quantity cancelled_shares;
};

struct OrderDeleted {
    EventHeader header;
    OrderRefNumber order_ref;
};

struct OrderReplaced {
    EventHeader header;
    OrderRefNumber old_order_ref;
    OrderRefNumber new_order_ref;
    Quantity shares;
    Price price;  // 4 d.p.
};

struct NonDisplayableOrderTrade {
    EventHeader header;
    OrderRefNumber order_ref;  // zero
    Side side;                 // 'B' or 'S' in raw data
    Quantity shares;
    std::string symbol;
    Price price;
    MatchNumber match_number;
};

struct CrossTrade {
    EventHeader header;
    Quantity shares;
    std::string symbol;
    Price cross_price;  // 4 d.p.
    MatchNumber match_number;
    char cross_type;  // 'O': opening cross, 'C': closing cross, 'H': ipo / halted / paused cross
};

struct BrokenTrade {
    EventHeader header;
    MatchNumber match_number;
};

// clang-format off
using ItchMessage = std::variant<
    Order,
    OrderExecuted,
    OrderExecutedWithPrice,
    OrderCancelled,
    OrderDeleted,
    OrderReplaced,
    NonDisplayableOrderTrade,
    CrossTrade, 
    BrokenTrade
>;
// clang-format on

}  // namespace bmp::core

#endif
