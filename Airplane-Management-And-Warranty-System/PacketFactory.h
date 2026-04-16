#pragma once

#include <functional>
#include <unordered_map>

#include <crow/json.h>

#include "Packet.h"
#include "SequenceCounter.h"
#include "ErrorCode.h"


// Creates packets of specific types and assembles the payload using JSON
class PacketFactory
{
    static Packet Packetize(PacketType type, uint32_t sequence, crow::json::wvalue& body);
public:
    // Authentication
    static Packet Handshake(uint32_t sequence, const std::string& technicianID, const std::string& name);
    static Packet HandshakeAck(uint32_t sequence, bool success, const std::string& sessionToken = "");
   
    // Maintenance Event Submission
    static Packet MaintenanceEvent(uint32_t sequence, int airplaneID, const std::string& technician, const std::string& eventType, const std::string& description);

    //Warranty Event Submission
    static Packet WarrantyEvent(uint32_t sequence, int airplaneID, const std::string& technicianID, int warrantyID, const std::string& description, const std::string& imageBytes);

    // Aircraft Record History
    static Packet MaintenanceHistory(uint32_t sequence, int airplaneID);
    static Packet QueryResponse(uint32_t sequence, int totalCount, const std::string& records);

    // Warranty Record
    static Packet ReportRequest(uint32_t sequence, int airplaneID);
    static Packet ReportData(uint32_t sequence, int airplaneID, const std::string& reportData);

    static Packet Ack(uint32_t sequence);
    static Packet Error(uint32_t sequence, ErrorCode code, const std::string& detail);
};