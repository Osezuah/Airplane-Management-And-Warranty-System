#include "SequenceCounter.h"

uint32_t SequenceCounter::next() {
	return sequence.fetch_add(1, std::memory_order_relaxed);
}

void SequenceCounter::reset() {
	sequence.store(1, std::memory_order_relaxed);
}