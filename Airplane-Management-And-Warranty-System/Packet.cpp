#include "Packet.h"

PacketType Packet::getType()
{
	return PacketType();
}

uint32_t Packet::getSequence()
{
	return 0;
}

uint64_t Packet::getTimestampMiliseconds()
{
	return 0;
}

uint32_t Packet::payloadSize()
{
	return 0;
}
