#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <libpq-fe.h>
#pragma comment(lib, "libpq.lib")

#include <string>
#include <iostream>
#include <vector>
#include "Packet.h"
#include "PacketFactory.h"
#include "StateMachine.h"
#include "Logger.h"
#include "crow.h" 

#define PAGE_SIZE 4096

int main() {
	Logger logger;
	StateMachine stateMachine;

	// Connect to the PostgreSQL database on docker v17.4
	//Airplane Management and Warranty System Server
	// Connect to the PostgreSQL database v17.4
	PGconn* conn = PQconnectdb("host=127.0.0.1 port=5432 dbname=amws user=postgres password=Lkj876*bv");
	if (PQstatus(conn) != CONNECTION_OK) {
		logger.Log("Connection to database failed: " + std::string(PQerrorMessage(conn)));
		PQfinish(conn);
		return 1;
	}

	// Setup Server through Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		logger.Log("Can't initialize winsock!");
		return 1;
	}


	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		logger.Log("Invalid socket.");
		WSACleanup();
		return 1;
	}

	// Bind socket to address
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(27000);

	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		logger.Log("Failed to bind socket.");
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	// Server listens on a socket
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		logger.Log("Socket failed to listen.");
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	logger.Log("Server setup finished. Server ON. State: " + stateMachine.StateToString());

	// Loop to keep the server constantly running as new connections are accepted
	bool listening = true;
	while (listening) {
		// Reset state machine if not already in new connection
		if (stateMachine.GetState() != ServerState::IDLE)
			stateMachine.TransitionStateTo(ServerState::IDLE);

		std::cout << "Waiting for client connection...\n" << std::endl;

		SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
		// Skip client on failed handshake
		if (clientSocket == INVALID_SOCKET) continue;

		// Transition state on client connection
		stateMachine.TransitionStateTo(ServerState::WAITING_FOR_VERIFICATION);
		logger.Log("Client connected and waiting for verification. State: " + stateMachine.StateToString());

		// Client work loop
		bool connected = true;
		while (connected) {
			//std::vector<uint8_t> rxBuffer(PAGE_SIZE);
			//int bytesReceived = recv(clientSocket, (char*)rxBuffer.data(), rxBuffer.size(), 0);

			// When client ends connection
			/*if (bytesReceived <= 0) {
				logger.Log("Client disconnected naturally.");
				stateMachine.TransitionStateTo(ServerState::IDLE);
				connected = false;
				break;
			}*/
			uint8_t headerBuffer[PACKETHEADER_BYTE_SIZE];
			int totalHeaderRead = 0;

			while (totalHeaderRead < PACKETHEADER_BYTE_SIZE) {
				int r = recv(clientSocket, (char*)headerBuffer + totalHeaderRead, PACKETHEADER_BYTE_SIZE - totalHeaderRead, 0);
				if (r <= 0) 
				{
					logger.Log("Client disconnected naturally.");
					stateMachine.TransitionStateTo(ServerState::IDLE);
					connected = false;
					break;
				};
				totalHeaderRead += r;
			}

			// Parse the header to see how much more data(payload) is coming
			PacketHeader* headerPtr = reinterpret_cast<PacketHeader*>(headerBuffer);
			uint32_t bodySize = headerPtr->payloadLength;
			uint32_t totalExpectedSize = PACKETHEADER_BYTE_SIZE + bodySize;

			// Create a buffer for the WHOLE packet
			std::vector<uint8_t> fullPacketBuffer(totalExpectedSize);

			// Copy the header we already have into the start of the full buffer
			std::memcpy(fullPacketBuffer.data(), headerBuffer, PACKETHEADER_BYTE_SIZE);

			// Loop to receive the remaining body bytes
			int currentBytesRead = PACKETHEADER_BYTE_SIZE;
			while (currentBytesRead < totalExpectedSize) {
				int r = recv(clientSocket, (char*)fullPacketBuffer.data() + currentBytesRead, totalExpectedSize - currentBytesRead, 0);
				if (r <= 0) 
				{
					logger.Log("Client disconnected naturally.");
					stateMachine.TransitionStateTo(ServerState::IDLE);
					connected = false;
					break;
				};
				currentBytesRead += r;
			}

			// Handle actual packet work
			try {
				Packet inputPacket = Packet::Deserialize(fullPacketBuffer.data(), currentBytesRead, false);
				crow::json::rvalue data = crow::json::load(inputPacket.payloadString());
				Packet outputPacket;
				if (!data) {
					outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::BAD_REQUEST, "Inlvalid JSON.");
					logger.Log("Invalid JSON, Request not processed.");
					stateMachine.TransitionStateTo(ServerState::ERROR_STATE);
				}

				// Behaviour dependant on current state
				switch (stateMachine.GetState()) {
					case ServerState::WAITING_FOR_VERIFICATION:
						if (inputPacket.getType() == PacketType::HANDSHAKE) {
							// Authenticate technician
							std::string technicianID = std::to_string(data["technicianID"].i());

							const char* command = "SELECT TechnicianID FROM Technician WHERE TechnicianID = $1";
							const char* parameters[1] = { technicianID.c_str() };
							PGresult* result = PQexecParams(conn, command, 1, NULL, parameters, NULL, NULL, 0);

							if (PQntuples(result) == 0) {
								// Technician ID not found in database
								logger.Log("Authentication Failed. Technician ID " + technicianID + " not found.");
								outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::UNAUTHORIZED, "Unknown Technician ID.");
								PQclear(result);
							}
							else {
								// Authenticate technician
								PQclear(result);
								stateMachine.TransitionStateTo(ServerState::CONNECTED);
								logger.Log("Handshake authenticated. State: " + stateMachine.StateToString());
								outputPacket = PacketFactory::HandshakeAck(inputPacket.getSequence(), true, "SESSION_VALID");
							}
						}
						else {
							logger.Log("Failed to authenticate. Wrong packet type received.");
							stateMachine.TransitionStateTo(ServerState::ERROR_STATE);
							outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::UNAUTHORIZED, "Invalid packet");
						}
						break;

					case ServerState::CONNECTED:
						// Process request when task is received
						stateMachine.TransitionStateTo(ServerState::PROCESSING_REQUEST);
						logger.Log("Task received. State: " + stateMachine.StateToString());

						if (inputPacket.getType() == PacketType::MAINTENANCE_EVENT) {
							logger.Log("Processing Maintenance Event.");

							// extract values
							std::string technicianID = std::to_string(data["technicianID"].i());
							std::string airplaneID = std::to_string(data["airplaneID"].i());
							std::string eventType = data["eventType"].s();
							std::string description = data["description"].s();

							// Handle writing to DB
							const char* command = "INSERT INTO MaintenanceEvent (TechnicianID_FK, AirplaneID_FK, Type, Description) VALUES ($1, $2, $3, $4)";
							const char* parameters[4] = { technicianID.c_str(), airplaneID.c_str(), eventType.c_str(), description.c_str() };
							PGresult* result = PQexecParams(conn, command, 4, NULL, parameters, NULL, NULL, 0);
							if (PQresultStatus(result) != PGRES_COMMAND_OK) {
								logger.Log("DB Error: " + std::string(PQerrorMessage(conn)));
								outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::INTERNAL, "Could not write to DB.");
							}
							else {
								logger.Log("Wrote to DB. Data: " + technicianID + "/" + airplaneID + "/" + eventType + "/" + description);
								outputPacket = PacketFactory::Ack(inputPacket.getSequence());
							}
							PQclear(result);

							// Reset state
							stateMachine.TransitionStateTo(ServerState::CONNECTED);
						}
						else if (inputPacket.getType() == PacketType::WARRANTY_EVENT) {
							logger.Log("Processing Warranty Event.");
							std::cout << "processing warranty event" <<std::endl;

							//extract values
							std::string technicianID = std::to_string(data["technicianID"].i());
							std::string airplaneID = std::to_string(data["airplaneID"].i());
							std::string warrantyID = std::to_string(data["warrantyID"].i());
							std::string description = data["description"].s();
							std::string encoded_image = data["imageBytes"].s();

							//decode base64 bytes 
							std::string imageBytes = crow::utility::base64decode(encoded_image);

							if (imageBytes.empty()) {
								logger.Log("Base64 decoding failed");
								outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::INTERNAL, "Could not decode base64 image");
							}

							//Handle writing to DB
							const char* command = "INSERT INTO WarrantyEvent (WarrantyID_FK, TechnicianID_FK, AirplaneID_FK, Description, Image) VALUES ($1, $2, $3, $4, $5)";
							const char* parameters[5] = { warrantyID.c_str(), technicianID.c_str(), airplaneID.c_str(), description.c_str(), imageBytes.data() };
							int paramLengths[5] = { 0 ,0 ,0 ,0 ,static_cast<int>(imageBytes.size()) };
							int paramFormats[5] = { 0, 0, 0, 0, 1 }; // 1=binary 0=text
							PGresult* result = PQexecParams(conn, command, 5, NULL, parameters, paramLengths, paramFormats, 0);
							if (PQresultStatus(result) != PGRES_COMMAND_OK) {
								logger.Log("DB Error: " + std::string(PQerrorMessage(conn)));
								outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::INTERNAL, "Could not write to DB.");
							}
							else {
								logger.Log("Wrote to DB. Data: " + technicianID + "/" + airplaneID + "/" + warrantyID + "/" + description + "/" + imageBytes);
								outputPacket = PacketFactory::Ack(inputPacket.getSequence());
							}
							PQclear(result);

							// Reset state
							stateMachine.TransitionStateTo(ServerState::CONNECTED);
						}
						else if (inputPacket.getType() == PacketType::MAINTENANCE_HISTORY) {
							std::string airplaneID = std::to_string(data["airplaneID"].i());

							// Query DB for specific airplane from ID
							const char* command = "SELECT M.Type, M.Description FROM MaintenanceEvent M "
												  "WHERE M.AirplaneID_FK = $1";
							const char* parameters[1] = { airplaneID.c_str() };
							PGresult* result = PQexecParams(conn, command, 1, NULL, parameters, NULL, NULL, 0);

							if (PQntuples(result) > 0) {
								int rows = PQntuples(result);
								int cols = PQnfields(result);

								std::vector<crow::json::wvalue> MHList;

								for (int i = 0; i < rows; i++) {
									crow::json::wvalue mh; //maintenance history
									for (int j = 0; j < cols; j++) {
										// Get column name and value
										std::string colName = PQfname(result, j);
										std::string val = PQgetvalue(result, i, j);
										mh[colName] = val;
									}
									MHList.push_back(std::move(mh));
								}
								crow::json::wvalue historyPayload;
								historyPayload = std::move(MHList);
								std::string historyStr = historyPayload.dump();

								PQclear(result);
								std::vector<uint8_t> Payload(historyStr.begin(), historyStr.end());

								//send to client
								Packet responsePacket(PacketType::REPORT_DATA, 1, std::move(Payload));
								std::vector<uint8_t> out = responsePacket.Serialize();
								size_t totalToSend = out.size();
								size_t totalSent = 0;
								const char* bufPtr = (const char*)out.data();

								while (totalSent < totalToSend) {
									int sent = send(clientSocket, bufPtr + totalSent, totalToSend - totalSent, 0);
									if (sent == SOCKET_ERROR) {
										logger.Log("Maintenance history report send failed with error: " + std::to_string(WSAGetLastError()));
										break;
									}
									totalSent += sent;
								}
							}
							else {

							}

							stateMachine.TransitionStateTo(ServerState::CONNECTED);
						}
						else if (inputPacket.getType() == PacketType::REPORT_REQUEST) {
							// Retrieve warranty data from DB to client
							logger.Log("Processing Report Request.");

							std::string airplaneID = std::to_string(data["airplaneID"].i());

							const char* command =
								"SELECT W.Status, WE.Description "
								"FROM Warranty W "
								"LEFT JOIN WarrantyEvent WE ON W.WarrantyID = WE.WarrantyID_FK "
								"WHERE W.AirplaneID_FK = $1 ORDER BY WE.WEventID DESC LIMIT 1";
							const char* parameters[1] = { airplaneID.c_str() };

							PGresult* result = PQexecParams(conn, command, 1, NULL, parameters, NULL, NULL, 1);

							const char* query = "SELECT regexp_replace(encode(Image, 'base64'), '\\n', '', 'g') FROM WarrantyEvent WE "
												"LEFT JOIN Warranty W ON W.WarrantyID = WE.WarrantyID_FK "
												"WHERE W.AirplaneID_FK = $1 ORDER BY WE.WEventID DESC LIMIT 1";
							PGresult* imageResult = PQexecParams(conn, query, 1, NULL, parameters, NULL, NULL, 0);

							if (PQntuples(result) > 0 && PQntuples(imageResult) > 0) {
								std::string status = PQgetvalue(result, 0, 0);
								std::string description = PQgetvalue(result, 0, 1);
								std::string base64Image = PQgetvalue(imageResult, 0, 0);

								// format json
								crow::json::wvalue payload;
								payload["status"] = status;
								payload["description"] = description;
								payload["image"] = base64Image;

								std::string payloadStr = "{\"status\":\"" + status + "\",\"description\":\"" + description + "\",\"image\":\"" + base64Image + "\"}";
								std::vector<uint8_t> binaryPayload(payloadStr.begin(), payloadStr.end());

								// send response
								Packet responsePacket(PacketType::REPORT_DATA, 1, std::move(binaryPayload));
								std::vector<uint8_t> out = responsePacket.Serialize();
								size_t totalToSend = out.size();
								size_t totalSent = 0;
								const char* bufPtr = (const char*)out.data();

								while (totalSent < totalToSend) {
									int sent = send(clientSocket, bufPtr + totalSent, totalToSend - totalSent, 0);
									if (sent == SOCKET_ERROR) {
										logger.Log("Warranty history report send failed with error: " + std::to_string(WSAGetLastError()));
										break;
									}
									totalSent += sent;
								}
								logger.Log("Warranty history report packet: Successfully sent " + std::to_string(totalSent) + " bytes to Client");
							}
							else {
							
							}

							PQclear(result);
							stateMachine.TransitionStateTo(ServerState::CONNECTED);
						}
						else {
							outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::BAD_REQUEST, "Unknown request.");
							stateMachine.TransitionStateTo(ServerState::CONNECTED);
						}
						break;

					case ServerState::ERROR_STATE:
						break;

					default:
						stateMachine.TransitionStateTo(ServerState::ERROR_STATE);
						outputPacket = PacketFactory::Error(inputPacket.getSequence(), ErrorCode::INTERNAL, "Invalid server state.");
						break;
				}

				// Send response to client
				std::vector<uint8_t> txData = outputPacket.Serialize();
				send(clientSocket, (char*)txData.data(), txData.size(), 0);
			}
			catch (const std::exception& e) {
				// Handle error from any state
				logger.Log("Exception caught: " + std::string(e.what()));
				stateMachine.TransitionStateTo(ServerState::ERROR_STATE);

				// Format error packet and send
				Packet errorPacket = PacketFactory::Error(0, ErrorCode::UNPROCESSABLE, "Packet data unnable to be processed.");
				std::vector<uint8_t> errorData = errorPacket.Serialize();
				send(clientSocket, (char*)errorData.data(), errorData.size(), 0);
			}

			// Reset server connection if on error state
			if (stateMachine.GetState() == ServerState::ERROR_STATE) {
				logger.Log("Server error triggered. Resetting connection...");

				stateMachine.TransitionStateTo(ServerState::IDLE);
				connected = false;
			}
		}
		closesocket(clientSocket);
	}
	closesocket(serverSocket);
	WSACleanup();
	PQfinish(conn);
	return 0;
}