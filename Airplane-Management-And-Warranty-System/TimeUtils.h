#pragma once

#include <cstdint>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std::chrono;

class TimeUtils
{
public:
	// this class cannot be instantiated
	TimeUtils() = delete;

	// Current timestamp in miliseconds
	static int64_t now_ms();

    // Returns input time as string in the following format:
    // Year-Month-Day-Hour:Minute:Second.Milisecond
	static std::string TimeToString(int64_t miliseconds);
};