#pragma once
// Test-only stand-in for MAIET's MUID, so the queue logic can be compiled on
// Linux without Windows/DirectX. The game continues using its real MUID.h.
struct MUID
{
    unsigned High = 0;
    unsigned Low = 0;
    MUID() = default;
    MUID(unsigned high, unsigned low) : High(high), Low(low) {}
    bool operator==(const MUID& rhs) const
    {
        return High == rhs.High && Low == rhs.Low;
    }
    bool operator!=(const MUID& rhs) const { return !(*this == rhs); }
};
