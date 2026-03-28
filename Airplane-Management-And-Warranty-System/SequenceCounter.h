#pragma once

#include <cstdint>
#include <atomic>

// Atomic sequence counter for packets
class SequenceCounter
{
	std::atomic<uint32_t> sequence = 1;
public:
	// Returns the current sequence, then increments it
	uint32_t next();
	void reset();
};