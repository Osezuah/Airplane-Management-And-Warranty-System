#include "PacketFactory.h"

Packet PacketFactory::Packetize(PacketType type, uint32_t sequence, crow::json::wvalue& body)
{
    std::string jsonString = body.dump();
    std::vector<uint8_t> bytes(jsonString.begin(), jsonString.end());

    return Packet(type, sequence, bytes);
}

Packet PacketFactory::Handshake(uint32_t sequence, const std::string& technicianID, const std::string& name)
{
    crow::json::wvalue body;
    body["technicianID"] = technicianID;
    body["name"] = name;

    return Packetize(PacketType::HANDSHAKE, sequence, body);
}

Packet PacketFactory::HandshakeAck(uint32_t sequence, bool success, const std::string& sessionToken)
{
    crow::json::wvalue body;
    body["success"] = success;
    body["sessionToken"] = sessionToken;

    return Packetize(PacketType::HANDSHAKE_ACK, sequence, body);
}

Packet PacketFactory::MaintenanceEvent(uint32_t sequence, int airplaneID, const std::string& technicianID, const std::string& eventType, const std::string& description)
{
    crow::json::wvalue body;
    body["airplaneID"] = airplaneID;
    body["technicianID"] = technicianID;
    body["eventType"] = eventType;
    body["description"] = description;

    return Packetize(PacketType::MAINTENANCE_EVENT, sequence, body);
}

Packet PacketFactory::QueryRequest(uint32_t sequence, int airplaneID)
{
    crow::json::wvalue body;
    body["airplaneID"] = airplaneID;

    return Packetize(PacketType::QUERY_REQUEST, sequence, body);
}

Packet PacketFactory::QueryResponse(uint32_t sequence, int totalCount, const std::string& records)
{
    crow::json::wvalue body;
    body["totalCount"] = totalCount;
    body["records"] = records;

    return Packetize(PacketType::QUERY_RESPONSE, sequence, body);
}

Packet PacketFactory::ReportRequest(uint32_t sequence, int airplaneID)
{
    crow::json::wvalue body;
    body["airplaneID"] = airplaneID;

    return Packetize(PacketType::REPORT_REQUEST, sequence, body);
}

Packet PacketFactory::ReportData(uint32_t sequence, int airplaneID, const std::string& reportData)
{
    crow::json::wvalue body;
    body["airplaneID"] = airplaneID;
    body["reportData"] = reportData;

    return Packetize(PacketType::REPORT_DATA, sequence, body);
}

Packet PacketFactory::Ack(uint32_t sequence)
{
    return Packet(PacketType::ACK, sequence, std::vector<uint8_t>{});
}

Packet PacketFactory::Error(uint32_t sequence, ErrorCode code, const std::string& detail)
{
    crow::json::wvalue body;
    body["code"] = static_cast<int32_t>(code);
    body["detail"] = detail;

    return Packetize(PacketType::ERROR_PACKET, sequence, body);
}
