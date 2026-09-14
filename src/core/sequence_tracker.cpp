#include "bmp/core/sequence_tracker.hpp"

#include <algorithm>

using bmp::core::SequenceResult;
using bmp::core::SequenceTracker;

SequenceTracker::SequenceTracker(std::string_view session) : session_ { session } {}

using enum bmp::core::SequenceStatus;

SequenceResult SequenceTracker::register_sequence_number(std::string_view session, std::uint64_t sequence,
                                                         std::uint16_t count) {
    if (session != session_) return { 0, SessionMismatch };
    if (count == 65535) terminated = true;
    if (terminated) return { 0, Terminated };
    if (count == 0) {
        if (!receivingStarted) {
            receivingStarted   = true;
            nextSequenceNumber = sequence;
            return { 0, Heartbeat };
        }
        if (sequence > nextSequenceNumber) return { 0, Gap };
        return { 0, Heartbeat };
    }
    if (!receivingStarted) {
        receivingStarted   = true;
        nextSequenceNumber = sequence + count;
        return { 0, Accepted };
    }
    if (sequence > nextSequenceNumber) return { 0, Gap };

    std::uint64_t packetsToSkip { std::min(nextSequenceNumber - sequence, static_cast<uint64_t>(count)) };
    nextSequenceNumber = std::max(nextSequenceNumber, sequence + count);

    return { static_cast<uint16_t>(packetsToSkip), Accepted };
}

std::uint64_t SequenceTracker::get_next_sequence_number() const {
    return nextSequenceNumber;
}
