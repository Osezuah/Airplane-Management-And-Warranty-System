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

#define PAGE_SIZE 4096

void send_all_airplanes_to_client(SOCKET clientSocket, PGconn* conn, Packet request) {
	// Database logic
	PGresult* res = PQexec(conn, "SELECT row_to_json(t) FROM (SELECT * FROM Airplane) t;");
	int rows = PQntuples(res);

	std::string records = "[";
	for (int i = 0; i < rows; i++) {
		records += PQgetvalue(res, i, 0);
		if (i < rows - 1) records += ",";
	}
	records += "]";
	PQclear(res);
	std::cout << records << std::endl;

	// Send the response back
	Packet response = PacketFactory::QueryResponse(request.getSequence(), rows, records);
	std::vector<uint8_t> out = response.Serialize();
	send(clientSocket, (const char*)out.data(), (int)out.size(), 0);
}

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
			std::vector<uint8_t> rxBuffer(PAGE_SIZE);
			int bytesReceived = recv(clientSocket, (char*)rxBuffer.data(), rxBuffer.size(), 0);

			// When client ends connection
			if (bytesReceived <= 0) {
				logger.Log("Client disconnected naturally.");
				stateMachine.TransitionStateTo(ServerState::IDLE);
				connected = false;
				break;
			}

			// Handle actual packet work
			try {
				Packet inputPacket = Packet::Deserialize(rxBuffer.data(), bytesReceived, false);
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
						else if (inputPacket.getType() == PacketType::QUERY_REQUEST) {
							std::string airplaneID = std::to_string(data["airplaneID"].i());

							// Query DB for specific airplane from ID
							const char* command = "SELECT * FROM Airplane WHERE AirplaneID = $1";
							const char* parameters[1] = { airplaneID.c_str() };
							PGresult* result = PQexecParams(conn, command, 1, NULL, parameters, NULL, NULL, 0);

							if (PQntuples(result) > 0) {
								// TO-DO: RETRIEVE DATA FROM DB AND SEND TO CLIENT
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
								"SELECT W.Status, WE.Description, WE.Image "
								"FROM Warranty W "
								"LEFT JOIN WarrantyEvent WE ON W.WarrantyID = WE.WarrantyID_FK "
								"WHERE W.AirplaneID_FK = $1 ORDER BY WE.WEventID DESC LIMIT 1";
							const char* parameters[1] = { airplaneID.c_str() };
							// Result format set to binary due to image data
							PGresult* result = PQexecParams(conn, command, 1, NULL, parameters, NULL, NULL, 1);

							if (PQntuples(result) > 0) {
								// TO-DO: RETRIEVE DATA FROM DB AND SEND TO CLIENT
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