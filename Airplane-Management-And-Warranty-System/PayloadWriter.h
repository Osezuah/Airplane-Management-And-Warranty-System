#pragma once

#include <cstdint>
#include <string>
#include <vector>

class PayloadWriter
{
	std::vector<uint8_t> buffer;

public:

	void WriteString(const std::string& value) {
		uint32_t length = static_cast<uint32_t>(value.size());
	}
};

