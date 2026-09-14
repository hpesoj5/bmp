#include "bmp/protocols/itch/itch_decoder.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <variant>

#include <gtest/gtest.h>

using namespace bmp::itch;

// clang-format off
static constexpr std::array<std::byte, 36> kAddOrderFixture {
    // type: A, stock locate: 0, tracking number: 0
    std::byte{0x41}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},

    // timestamp: 1000
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x03}, std::byte{0xE8},

    // order ref: 1
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},

    // side: B
    std::byte{0x42},

    // shares: 100
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x64}, 

    // symbol: AAPL
    std::byte{0x41}, std::byte{0x41}, std::byte{0x50}, std::byte{0x4C}, std::byte{0x20}, std::byte{0x20}, std::byte{0x20}, std::byte{0x20},

    // price: 1000500
    std::byte{0x00}, std::byte{0x0F}, std::byte{0x44}, std::byte{0x34}
};
// clang-format on

TEST(ItchDecoderTest, DecodesAddOrder) {
    ItchDecoder decoder;
    auto result { decoder.decode(kAddOrderFixture) };

    ASSERT_EQ(result.status, DecodeStatus::Ok);
    ASSERT_TRUE(result.message.has_value());
    ASSERT_TRUE(std::holds_alternative<Order>(result.message.value()));

    const auto& order { std::get<Order>(result.message.value()) };
    EXPECT_EQ(order.header.stock_locate, 0);
    EXPECT_EQ(order.header.tracking_number, 0);
    EXPECT_EQ(order.header.timestamp.count(), 1000);
    EXPECT_EQ(order.order_ref, 1);
    EXPECT_EQ(order.side, Side::Buy);
    EXPECT_EQ(order.shares, 100);
    EXPECT_EQ(order.symbol, "AAPL");
    EXPECT_EQ(order.price, 1000500);
}

TEST(ItchDecoderTest, TruncatedBufferReturnsTruncated) {
    ItchDecoder decoder;

    auto truncated { std::span(kAddOrderFixture).first(20) };

    auto result { decoder.decode(truncated) };

    EXPECT_EQ(result.status, DecodeStatus::Truncated);
    EXPECT_FALSE(result.message.has_value());
}

TEST(ItchDecoderTest, EmptyBufferReturnsTruncated) {
    ItchDecoder decoder;

    auto result { decoder.decode(std::span<const std::byte> {}) };

    EXPECT_EQ(result.status, DecodeStatus::Truncated);
    EXPECT_FALSE(result.message.has_value());
}

TEST(ItchDecoderTest, UnknownMessageTypeReturnsUnknownType) {
    ItchDecoder decoder;
    auto fixture { kAddOrderFixture };
    fixture[0] = std::byte { 0x5A };  // Z

    auto result { decoder.decode(fixture) };

    EXPECT_EQ(result.status, DecodeStatus::UnknownType);
    EXPECT_FALSE(result.message.has_value());
}

TEST(ItchDecoderTest, InvalidSideByteReturnsParseError) {
    ItchDecoder decoder;
    auto fixture { kAddOrderFixture };
    fixture[19] = std::byte { 0x58 };  // X

    auto result { decoder.decode(fixture) };

    EXPECT_EQ(result.status, DecodeStatus::ParseError);
    EXPECT_FALSE(result.message.has_value());
}
