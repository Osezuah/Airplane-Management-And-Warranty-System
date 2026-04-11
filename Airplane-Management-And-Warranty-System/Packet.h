#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

#include "TimeUtils.h"

# define PACKETHEADER_BYTE_SIZE 17

enum class PacketType : uint8_t {
    HANDSHAKE = 0x01,         // Client -> Server / Request for connection and authentication
    HANDSHAKE_ACK = 0x02,     // Servr -> Client / Server accepted and authenticated client
    MAINTENANCE_EVENT = 0x10, // Client -> Server / Client submits maintenance report
    WARRANTY_EVENT = 0x20,    // Client -> Server / Client submits warranty report
    QUERY_REQUEST = 0x25,     // Client -> Server / Client request airlplane record
    QUERY_RESPONSE = 0x21,    // Server -> Client / Server responds with record
    REPORT_REQUEST = 0x30,    // Client -> Server / Client requests warranty record
    REPORT_DATA = 0x31,       // Server -> Client / Server responds with warranty
    ACK = 0x0F,               // Both ways
    ERROR_PACKET = 0xFF              // Both ways
};

#pragma pack(push, 1) // prevents compiler padding ensuring header size
struct PacketHeader {
    uint8_t packetType;
    uint32_t sequenceNumber;
    int64_t timestamp;         // Unix epoch millisecond
    uint32_t payloadLength;     // Bytes in body
};
#pragma pack(pop)

// Checks (at compile time) if compiler added padding, even though it shouldn't
static_assert(sizeof(PacketHeader) == PACKETHEADER_BYTE_SIZE,
    "PACKET ERROR: Packet compiled with padding. Packet size must be 17 bytes, but was compiled as larger.");

class Packet
{
public:
    PacketHeader header{};
    std::vector<uint8_t> payload;

    Packet();

    Packet(PacketType type, uint32_t sequence, std::vector<uint8_t> payloadData);

    std::vector<uint8_t> Serialize();

    static Packet Deserialize(const uint8_t* data, size_t size, bool headerOnly);

    // accessors
    PacketType getType() const;
    uint32_t getSequence() const;
    uint64_t getTimestampMiliseconds() const;
    uint32_t payloadSize() const;
    std::string payloadString() const;
};

