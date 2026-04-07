#include "Packet.h"


Packet::Packet()
{
	this->header.packetType = static_cast<uint8_t>(PacketType::PACKET_ERROR);
	this->header.payloadLength = 0;
	this->header.sequenceNumber = 0;
	this->header.timestamp = 0;
	this->payload.clear();
}

Packet::Packet(PacketType type, uint32_t sequence, std::vector<uint8_t> payloadData)
{
	this->payload = std::move(payloadData);

	if (payload.size() > UINT32_MAX)
		throw std::length_error("Payload too large.");

	this->header.packetType = static_cast<uint8_t>(type);
	this->header.sequenceNumber = sequence;
	this->header.timestamp = TimeUtils::NowMs();
	this->header.payloadLength = static_cast<uint32_t>(payload.size());
}

std::vector<uint8_t> Packet::Serialize()
{
	std::vector<uint8_t> buffer(PACKETHEADER_BYTE_SIZE + payload.size());
	std::memcpy(buffer.data(), &header, PACKETHEADER_BYTE_SIZE);
	if (!payload.empty())
		std::memcpy(buffer.data() + PACKETHEADER_BYTE_SIZE, payload.data(), payload.size());
	return buffer;
}

Packet Packet::Deserialize(const uint8_t* data, size_t size, bool headerOnly)
{
	if (size < PACKETHEADER_BYTE_SIZE)
		throw std::runtime_error("Buffer too small to contain header.");

	// Copy header
	Packet packet;
	std::memcpy(&packet.header, data, PACKETHEADER_BYTE_SIZE);

	if (headerOnly) {
		return packet;
	}

	// Check for payload length accuracy
	const size_t expected = PACKETHEADER_BYTE_SIZE + packet.header.payloadLength;
	if (size < expected)
		throw std::runtime_error("Received less payload bytes than expected.");

	// Deserialize payload, reading only what is after header
	packet.payload.assign(
		data + PACKETHEADER_BYTE_SIZE,
		data + PACKETHEADER_BYTE_SIZE + packet.header.payloadLength);

	return packet;
}

PacketType Packet::getType() const
{
	return static_cast<PacketType>(this->header.packetType);
}

uint32_t Packet::getSequence() const
{
	return this->header.sequenceNumber;
}

uint64_t Packet::getTimestampMiliseconds() const
{
	return this->header.timestamp;
}

uint32_t Packet::payloadSize() const
{
	return static_cast<uint32_t>(PACKETHEADER_BYTE_SIZE + payload.size());
}

std::string Packet::payloadString() const
{
	/*const uint8_t* rawBytes = payload.data();
	const char* asChar = reinterpret_cast<const char*>(rawBytes);
	std::string asString(asChar, payload.size());
	return asString;*/
	std::string s(payload.begin(), payload.end());
	// Remove any trailing null terminators that might have snuck in from the network buffer
	s.erase(std::find(s.begin(), s.end(), '\0'), s.end());
	return s;
}
