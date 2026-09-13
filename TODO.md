# TODO List

## Stage 1
1. `message_types`
  - structs for each message type, exchange-agnostic (e.g. generic `Order`, `Trade`)
  - look what fields ITCH and OUCH have in common before designing
2. `byte_reader`
  - safe primitives for reading fixed-width integers from a raw byte buffer with arbitrary offsets, with endian conversion
  - `memcpy` then swap, `std::endian`, `std::byteswap`

## Stage 2
1. `decoder_interface`
  - CRTP base taking a byte span and returning a message type
  - `std::span<const std::byte>` as input type for zero-copy
2. `ring_buffer`
  - single threaded: preallocated buffer to hold incoming bytes without heap allocation
  - multithreaded: SPSC ring buffer, lock-free or not lock-free?
3. `sequence_tracker`
  - detects gaps in message sequence numbers to request dropped messages
  - research how ITCH and OUCH expose sequence numbers in their headers, and what a typical gap-fill/retransmission-request mechanism looks like

## Stage 3
1. `itch_constants`
  - message type byte values, field widths and offsets, enum values for order sides etc.
2. `itch_messages`
  - matches ITCH spec layout exactly
  - `#pragma pack(1)` or `alignas(1)` if packed structs are desired, otherwise read fields positionally via `byte_reader`
3. `itch_decoder`
  - `IMessageDecoder` for ITCH
4. `test_itch_decoder`
  - verify decoder against actual captured bytes
  - use NASDAQ sample ITCH data files

## Stage 4
1. `ouch_decoder`
  - may also need an encoder since it is bidirectional for acknowledgement/reject/fill responses returned by exchange
2. `test_ouch_decoder`

## Stage 5
1. `framing`
  - logic for splitting stream of received bytes into individual messages
  - research whether ITCH messages arrive one-per-UDP packet or batched
2. `udp_multicast`
  - joining a multicast group to receive ITCH-style market data
3. `tcp_session`
  - TCP connection handling OUCH order entry

## Stage 6
1. `decoder_factory`
  - picks ITCH or OUCH decoder based on config, selected at startup
2. `config_loader`
  - reads which protocol/host/port to connect from a config file or CLI args
  - JSON/TOML lib, or something else
3. `main`

## Stage 7
`bench_decode_throughput`
