#define _CRT_SECURE_NO_WARNINGS
#include "TimeUtils.h"

int64_t TimeUtils::NowMs() {
    return static_cast<uint64_t>(
        duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()).count());
}

std::string TimeUtils::TimeToString(int64_t miliseconds) {
    system_clock::time_point timePoint = system_clock::time_point(milliseconds(miliseconds));
    // Convert timePoint to seconds
    std::time_t seconds = system_clock::to_time_t(timePoint);
    // Format seconds to gmt time for string interpretation
    std::tm* gmtTime = std::gmtime(&seconds);

    std::stringstream ss;
    ss << std::put_time(gmtTime, "%Y-%m-%d-%H:%M:%S");
    // Appends milliseconds to timestamp, filling 0s for any empty numbers
    ss << '.' << std::setfill('0') << std::setw(3) << (miliseconds % 1000);
    return ss.str();
}