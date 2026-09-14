#include "bmp/protocols/itch/itch_decoder.hpp"

#include "bmp/core/byte_reader.hpp"
#include "bmp/protocols/itch/itch_constants.hpp"

using namespace bmp::itch;
using enum DecodeStatus;

ItchResult ItchDecoder::decode_impl(BufferView buf) const {
    if (buf.empty()) return { Truncated, std::nullopt };

    char message_type { static_cast<char>(buf.front()) };
    auto stock_locate { read_bytes<StockLocate>(buf, common::stock_locate.offset, common::stock_locate.length) };
    auto tracking_number { read_bytes<TrackingNumber>(buf, common::tracking_number.offset,
                                                      common::tracking_number.length) };
    auto nanosecondsSinceMidnight { read_bytes<std::uint64_t>(buf, common::timestamp.offset,
                                                              common::timestamp.length) };

    if (!stock_locate || !tracking_number || !nanosecondsSinceMidnight) return { Truncated, std::nullopt };

    nsSinceMidnight timestamp { static_cast<std::int64_t>(nanosecondsSinceMidnight.value()) };

    EventHeader header { message_type, stock_locate.value(), tracking_number.value(), timestamp };

    switch (message_type) {
    case 'A': {
        using namespace add_order;
        auto order_ref_ { read_bytes<OrderRefNumber>(buf, order_ref.offset, order_ref.length) };
        auto side_ { read_bytes<char>(buf, side.offset, side.length) };  // must convert to enum
        auto shares_ { read_bytes<Quantity>(buf, shares.offset, shares.length) };
        auto symbol_ { read_bytes<std::string>(buf, symbol.offset, symbol.length) };
        auto price_ { read_bytes<Price>(buf, price.offset, price.length) };

        if (!order_ref_ || !side_ || !shares_ || !symbol_ || !price_) return { Truncated, std::nullopt };
        if (side_.value() != 'B' && side_.value() != 'S') return { ParseError, std::nullopt };
        return {
            Ok, Order { header, order_ref_.value(), (side_.value() == 'B' ? Side::Buy : Side::Sell), shares_.value(),
                       std::move(symbol_.value()), price_.value() }
        };
    }

    case 'E': {
        using namespace order_executed;
        auto order_ref_ { read_bytes<OrderRefNumber>(buf, order_ref.offset, order_ref.length) };
        auto shares_ { read_bytes<Quantity>(buf, shares.offset, shares.length) };
        auto match_number_ { read_bytes<MatchNumber>(buf, match_number.offset, match_number.length) };

        if (!order_ref_ || !shares_ || !match_number_) return { Truncated, std::nullopt };
        return {
            Ok, OrderExecuted { header, order_ref_.value(), shares_.value(), match_number_.value() }
        };
    }

    case 'C': {
        using namespace order_executed;
        auto order_ref_ { read_bytes<OrderRefNumber>(buf, order_ref.offset, order_ref.length) };
        auto shares_ { read_bytes<Quantity>(buf, shares.offset, shares.length) };
        auto match_number_ { read_bytes<MatchNumber>(buf, match_number.offset, match_number.length) };
        auto printable_ { read_bytes<char>(buf, printable.offset, printable.length) };
        auto price_ { read_bytes<Price>(buf, price.offset, price.length) };

        if (!order_ref_ || !shares_ || !match_number_ || !printable_ || !price_) return { Truncated, std::nullopt };
        return {
            Ok, OrderExecutedWithPrice { header, order_ref_.value(), shares_.value(), match_number_.value(),
                                        printable_.value(), price_.value() }
        };
    }

    case 'X': {
        using namespace order_cancelled;
        auto order_ref_ { read_bytes<OrderRefNumber>(buf, order_ref.offset, order_ref.length) };
        auto shares_ { read_bytes<Quantity>(buf, shares.offset, shares.length) };

        if (!order_ref_ || !shares_) return { Truncated, std::nullopt };
        return {
            Ok, OrderCancelled { header, order_ref_.value(), shares_.value() }
        };
    }

    case 'D': {
        using namespace order_deleted;
        auto order_ref_ { read_bytes<OrderRefNumber>(buf, order_ref.offset, order_ref.length) };

        if (!order_ref_) return { Truncated, std::nullopt };
        return {
            Ok, OrderDeleted { header, order_ref_.value() }
        };
    }

    case 'U': {
        using namespace order_replaced;
        auto order_ref_ { read_bytes<OrderRefNumber>(buf, old_order_ref.offset, old_order_ref.length) };
        auto new_order_ref_ { read_bytes<OrderRefNumber>(buf, new_order_ref.offset, new_order_ref.length) };
        auto shares_ { read_bytes<Quantity>(buf, shares.offset, shares.length) };
        auto price_ { read_bytes<Price>(buf, price.offset, price.length) };

        if (!order_ref_ || !new_order_ref_ || !shares_ || !price_) return { Truncated, std::nullopt };
        return {
            Ok, OrderReplaced { header, order_ref_.value(), new_order_ref_.value(), shares_.value(), price_.value() }
        };
    }

    case 'P': {
        using namespace non_displayable_order_trade;
        auto order_ref_ { read_bytes<OrderRefNumber>(buf, order_ref.offset, order_ref.length) };
        auto side_ { read_bytes<char>(buf, side.offset, side.length) };  // must convert to enum
        auto shares_ { read_bytes<Quantity>(buf, shares.offset, shares.length) };
        auto symbol_ { read_bytes<std::string>(buf, symbol.offset, symbol.length) };
        auto price_ { read_bytes<Price>(buf, price.offset, price.length) };
        auto match_number_ { read_bytes<MatchNumber>(buf, match_number.offset, match_number.length) };

        if (!order_ref_ || !side_ || !shares_ || !symbol_ || !price_ || !match_number_)
            return { Truncated, std::nullopt };
        if (side_.value() != 'B' && side_.value() != 'S') return { ParseError, std::nullopt };
        return {
            Ok, NonDisplayableOrderTrade { header, order_ref_.value(), (side_.value() == 'B' ? Side::Buy : Side::Sell),
                                          shares_.value(), std::move(symbol_.value()), price_.value(),
                                          match_number_.value() }
        };
    }

    case 'Q': {
        using namespace cross_trade;
        auto shares_ { read_bytes<Quantity>(buf, shares.offset, shares.length) };
        auto symbol_ { read_bytes<std::string>(buf, symbol.offset, symbol.length) };
        auto price_ { read_bytes<Price>(buf, price.offset, price.length) };
        auto match_number_ { read_bytes<MatchNumber>(buf, match_number.offset, match_number.length) };
        auto cross_type_ { read_bytes<char>(buf, cross_type.offset, cross_type.length) };

        if (!shares_ || !symbol_ || !price_ || !match_number_ || !cross_type_) return { Truncated, std::nullopt };

        switch (cross_type_.value()) {
        case 'O':
            break;
        case 'C':
            break;
        case 'H':
            break;
        default:
            return { ParseError, std::nullopt };
        }

        return {
            Ok, CrossTrade { header, shares_.value(), std::move(symbol_.value()), price_.value(), match_number_.value(),
                            cross_type_.value() }
        };
    }

    case 'B': {
        using namespace broken_trade;
        auto match_number_ { read_bytes<MatchNumber>(buf, match_number.offset, match_number.length) };

        if (!match_number_) return { Truncated, std::nullopt };
        return {
            Ok, BrokenTrade { header, match_number_.value() }
        };
    }

    default:
        return { UnknownType, std::nullopt };
    }
}
