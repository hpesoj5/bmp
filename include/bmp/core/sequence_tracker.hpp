#pragma once
#ifndef SEQUENCE_TRACKER_H
#define SEQUENCE_TRACKER_H

#include <cstdint>
#include <string>

// works with Downstream Header
namespace bmp::core {

enum class SequenceStatus {
    Accepted,
    Gap,
    Heartbeat,
    SessionMismatch,
    Terminated,
};

struct SequenceResult {
    std::uint16_t skip {};
    SequenceStatus status;
};

class SequenceTracker {
public:
    explicit SequenceTracker(std::string_view session);
    SequenceResult register_sequence_number(std::string_view session, std::uint64_t sequence,
                                            std::uint16_t count);  // [number of packets to skip, register success]
    std::uint64_t get_next_sequence_number() const;

private:
    std::string session_;
    std::uint64_t nextSequenceNumber {};
    bool receivingStarted { false };
    bool terminated { false };
};

}  // namespace bmp::core
#endif
