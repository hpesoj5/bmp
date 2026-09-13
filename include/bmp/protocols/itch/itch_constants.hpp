#pragma once
#ifndef ITCH_CONSTANTS_H
#define ITCH_CONSTANTS_H

#include <cstddef>

namespace bmp::itch {

struct Values {
    std::size_t offset;
    std::size_t length;
};

namespace downstream::header {
    constexpr Values session { 0, 10 };
    constexpr Values sequence_number           = { 10, 8 };
    constexpr Values message_count             = { 18, 2 };
    constexpr std::size_t messageLength_length = 2;
}  // namespace downstream::header

namespace request {
    constexpr Values session         = { 0, 10 };
    constexpr Values sequence_number = { 10, 8 };
    constexpr Values message_count   = { 18, 2 };
}  // namespace request

namespace common {
    constexpr Values message_type    = { 0, 1 };
    constexpr Values stock_locate    = { 1, 2 };
    constexpr Values tracking_number = { 3, 2 };
    constexpr Values timestamp       = { 5, 6 };
}  // namespace common

namespace add_order {
    constexpr Values order_ref  = { 11, 8 };
    constexpr Values side       = { 19, 1 };
    constexpr Values shares     = { 20, 4 };
    constexpr Values symbol     = { 24, 8 };
    constexpr Values price      = { 32, 4 };
    constexpr Values attributor = { 36, 4 };
}  // namespace add_order

namespace order_executed {
    constexpr Values order_ref    = { 11, 8 };
    constexpr Values shares       = { 19, 4 };
    constexpr Values match_number = { 23, 8 };
    constexpr Values printable    = { 31, 1 };
    constexpr Values price        = { 32, 4 };
}  // namespace order_executed

namespace order_cancelled {
    constexpr Values order_ref = { 11, 8 };
    constexpr Values shares    = { 19, 4 };
}  // namespace order_cancelled

namespace order_deleted {
    constexpr Values order_ref = { 11, 8 };
}

namespace order_replaced {
    constexpr Values old_order_ref = { 11, 8 };
    constexpr Values new_order_ref = { 19, 1 };
    constexpr Values shares        = { 27, 4 };
    constexpr Values price         = { 31, 8 };
}  // namespace order_replaced

namespace non_displayable_order_trade {
    constexpr Values order_ref    = { 11, 8 };
    constexpr Values side         = { 19, 1 };
    constexpr Values shares       = { 20, 4 };
    constexpr Values symbol       = { 24, 8 };
    constexpr Values price        = { 32, 4 };
    constexpr Values match_number = { 36, 8 };
}  // namespace non_displayable_order_trade

namespace cross_trade {
    constexpr Values shares       = { 11, 8 };
    constexpr Values symbol       = { 19, 8 };
    constexpr Values price        = { 27, 4 };
    constexpr Values match_number = { 31, 8 };
    constexpr Values cross_type   = { 39, 1 };
}  // namespace cross_trade

namespace broken_trade {
    constexpr Values match_number = { 11, 8 };
}

};  // namespace bmp::itch

#endif
