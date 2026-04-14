#include "asio.hpp"
#include "crow.h"
#include <libpq-fe.h>
#include <winsock.h>
#include "..\Airplane-Management-And-Warranty-System\Packet.h"
#include "..\Airplane-Management-And-Warranty-System\PacketFactory.h"
#include "..\Airplane-Management-And-Warranty-System\Logger.h"

#define PAGE_SIZE 4096

//function to connect to server via TCP sockets and perform handshake, returning session token if successful
void handshake_with_tcp_server(SOCKET sock) {
	//send 
	Packet handshakePacket = PacketFactory::Handshake(1, "1", "Client");
	std::vector<uint8_t> txData = handshakePacket.Serialize();
	send(sock, (char*)txData.data(), txData.size(), 0);

	//recv
	std::vector<uint8_t> rxBuffer(PAGE_SIZE);
	int bytesReceived = recv(sock, (char*)rxBuffer.data(), rxBuffer.size(), 0);
	Packet inputPacket = Packet::Deserialize(rxBuffer.data(), bytesReceived, false);
	crow::json::rvalue data = crow::json::load(inputPacket.payloadString());

	if (!data) {
		std::cerr << "Invalid JSON received from TCP server." << std::endl;
	}
	std::string success = data["sessionToken"].s();
	std::cout << "Handshake response received. Session Token: " << success << std::endl;
}

//establish connection to TCP server and perform handshake, returning socket if successful
SOCKET establish_connection() {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) return INVALID_SOCKET;

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(27000);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(sock);
		return INVALID_SOCKET;
	}
	handshake_with_tcp_server(sock);

	return sock;
}


// terminate connection by closing socket
void terminate_connection(SOCKET s)
{
	closesocket(s);
}

//logging function to log messages to client_log.txt using Logger class
void logging(const std::string& message) {
	Logger logger("client_log.txt");
	logger.Log(message);
}

int main() {
	//copy http://localhost:8080 to search bar

	//Airplane Management and Warranty System API Webserver
	// Connect to the PostgreSQL database on docker v17.4
	PGconn* conn = PQconnectdb(
		"host=127.0.0.1 port=5432 dbname=amws user=postgres password=Lkj876*bv"
	);
	if (PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		return 1;
	}
	else {
		printf("Connected to database successfully!\n");
	}
	// Initialize the Crow application
	crow::SimpleApp app;

	// root route
	CROW_ROUTE(app, "/")([]() {
		auto page = crow::mustache::load_text("HTMLPage.html");
		return page;
	});

	//route to connect to server via TCP sockets
	CROW_ROUTE(app, "/connect-to-tcp-server")([]() {	
		SOCKET socket = establish_connection();
		if (socket == INVALID_SOCKET) {
			logging("Failed to connect to TCP server.");
			terminate_connection(socket);
			return crow::response(503, "Service Unavailable: TCP Server Down");
		}
		logging("Connected to TCP server and completed handshake.");
		terminate_connection(socket);
		logging("Disconnected from the TCP server; closed socket");
		return crow::response(200, "Connected to TCP Server");
	});

	//route to access airplanes in database
	CROW_ROUTE(app, "/airplanes")([conn]() {
			const char* command = "SELECT * FROM Airplane";
			PGresult* result = PQexec(conn, command);

			if (PQresultStatus(result) != PGRES_TUPLES_OK) {
				PQclear(result);
				return crow::response(500, "Database Query Failed");
			}

			int rows = PQntuples(result);
			int cols = PQnfields(result);

			std::vector<crow::json::wvalue> airplaneList;

			for (int i = 0; i < rows; i++) {
				crow::json::wvalue airplane;
				for (int j = 0; j < cols; j++) {
					// Get column name and value
					std::string colName = PQfname(result, j);
					std::string val = PQgetvalue(result, i, j);
					airplane[colName] = val;
				}
				airplaneList.push_back(std::move(airplane));
			}

			// Clean up and return
			PQclear(result);
			crow::json::wvalue finalResponse;
			finalResponse["data"] = std::move(airplaneList);

			logging("Retrieved all airplanes from database.");

			return crow::response(finalResponse);
	});

	CROW_ROUTE(app, "/maintenance_event").methods("POST"_method)([](const crow::request& req) {
		SOCKET sock = establish_connection();
		if (sock == INVALID_SOCKET) {
			logging("Failed to connect to TCP server.");
			terminate_connection(sock);
			return crow::response(503, "Service Unavailable: TCP Server Down");
		}
		logging("Connected to TCP server and completed handshake.");

		auto Body = crow::json::load(req.body);
		if (!Body || !Body.has("airplaneID") || !Body.has("technicianID")) {
			terminate_connection(sock);
			return crow::response(400, "Invalid JSON data received");
		}

		try {
			int airplaneID = Body["airplaneID"].i();
			std::string technicianID = Body["technicianID"].s();
			std::string type = Body["eventType"].s();
			std::string desc = Body["description"].s();

			//send
			Packet maintenancePacket = PacketFactory::MaintenanceEvent(1, airplaneID, technicianID, type, desc);
			std::vector<uint8_t> txData = maintenancePacket.Serialize();
			send(sock, (char*)txData.data(), txData.size(), 0);
			logging("Sent maintenance event packet to tcp server");

			//recv
			std::vector<uint8_t> rxBuffer(PAGE_SIZE);
			int bytesReceived = recv(sock, (char*)rxBuffer.data(), rxBuffer.size(), 0);
			logging("Received maintenance event packet acknowledgement from tcp server");

			terminate_connection(sock);
			logging("Disconnected from the TCP server; closed socket");
			return crow::response(200, "Maintenance Event sent successfully");
		}
		catch (const std::exception& e) {
			terminate_connection(sock);
			logging("Disconnected from the TCP server; closed socket");
			return crow::response(400, "Error processing request: " + std::string(e.what()));
		}
	});

	CROW_ROUTE(app, "/warranty_event").methods("POST"_method)([conn](const crow::request& req) {
		SOCKET sock = establish_connection();
		if (sock == INVALID_SOCKET) {
			logging("Failed to connect to TCP server.");
			terminate_connection(sock);
			return crow::response(503, "Service Unavailable: TCP Server Down");
		}
		logging("Connected to TCP server and completed handshake.");

		auto Body = crow::json::load(req.body);
		if (!Body || !Body.has("airplaneID") || !Body.has("technicianID")) {
			terminate_connection(sock);
			return crow::response(400, "Invalid JSON data received");
		}

		try {
			int airplaneID = Body["airplaneID"].i();
			std::string technicianID = Body["technicianID"].s();
			std::string encoded_image = Body["imageBytes"].s();
			std::string desc = Body["description"].s();
			int warrantyID = 0;

			const char* command = "SELECT WarrantyID FROM Warranty WHERE AirplaneID_FK = $1";
			std::string airplaneIDStr = std::to_string(airplaneID);
			const char* parameters[1] = { airplaneIDStr.c_str() };
			PGresult* result = PQexecParams(conn, command, 1, NULL, parameters, NULL, NULL, 0);
			if (PQntuples(result) > 0) {
				char* val = PQgetvalue(result, 0, 0);
				int warrantyID = std::stoi(val);
				std::cout << warrantyID << std::endl;

				//send
				Packet warrantyPacket = PacketFactory::WarrantyEvent(10, airplaneID, technicianID, warrantyID, desc, encoded_image);
				std::vector<uint8_t> txData = warrantyPacket.Serialize();
				size_t totalToSend = txData.size();
				size_t totalSent = 0;
				const char* bufPtr = (const char*)txData.data();

				while (totalSent < totalToSend) {
					int sent = send(sock, bufPtr + totalSent, totalToSend - totalSent, 0);
					if (sent == SOCKET_ERROR) {
						logging("Warranty event send failed with error: " + std::to_string(WSAGetLastError()));
						break;
					}
					totalSent += sent;
				}
				logging("Warranty event packet: Successfully sent " + std::to_string(totalSent) + " bytes to TCP server");

				//recv
				std::vector<uint8_t> rxBuffer(PAGE_SIZE);
				int bytesReceived = recv(sock, (char*)rxBuffer.data(), rxBuffer.size(), 0);
				logging("Received warranty event packet acknowledgement from tcp server");

				terminate_connection(sock);
				logging("Disconnected from the TCP server; closed socket");
				return crow::response(200, "Warranty Event sent successfully");
			}
			else {
				terminate_connection(sock);
				logging("Disconnected from the TCP server; closed socket");
				return crow::response(500, "WarrantyID not found in DB");
			}
		}
		catch (const std::exception& e) {
			terminate_connection(sock);
			logging("Disconnected from the TCP server; closed socket");
			return crow::response(400, "Error processing request: " + std::string(e.what()));
		}
	});

	CROW_ROUTE(app, "/warranty_history/<int>")([&](int id) {
		SOCKET sock = establish_connection();
		if (sock == INVALID_SOCKET) {
			logging("Failed to connect to TCP server.");
			terminate_connection(sock);
			return crow::response(503, "Service Unavailable: TCP Server Down");
		}
		try {
			int airplaneID = id;
			logging("Connected to TCP server and completed handshake.");

			// send report request
			Packet reportRequestPacket = PacketFactory::ReportRequest(1, airplaneID);
			std::vector<uint8_t> txData = reportRequestPacket.Serialize();
			send(sock, (char*)txData.data(), txData.size(), 0);
			logging("Sent warranty history report request packet to TCP server for AirplaneID: " + std::to_string(airplaneID));

			//recv report data
			uint8_t headerBuffer[PACKETHEADER_BYTE_SIZE] = { 0 };
			int totalHeaderRead = 0;

			while (totalHeaderRead < PACKETHEADER_BYTE_SIZE) {
				int r = recv(sock, (char*)headerBuffer + totalHeaderRead, PACKETHEADER_BYTE_SIZE - totalHeaderRead, 0);
				if (r <= 0)
				{
					logging("Server disconnected");
					terminate_connection(sock);
					break;
				};
				totalHeaderRead += r;
			}

			if (totalHeaderRead != PACKETHEADER_BYTE_SIZE) {
				logging("Error: Failed to receive full packet header");
				terminate_connection(sock);
				return crow::response(500, "Incomplete header received");
			}

			// Parse the header to see how much more data(payload) is coming
			PacketHeader* headerPtr = reinterpret_cast<PacketHeader*>(headerBuffer);
			uint32_t bodySize = headerPtr->payloadLength;
			uint32_t totalExpectedSize = PACKETHEADER_BYTE_SIZE + bodySize;

			// Create a buffer for the WHOLE packet
			std::vector<uint8_t> fullPacketBuffer(totalExpectedSize);

			// Copy the header we already have into the start of the full buffer
			std::memcpy(fullPacketBuffer.data(), headerBuffer, totalHeaderRead);

			// Loop to receive the remaining body bytes
			int currentBytesRead = PACKETHEADER_BYTE_SIZE;
			while (currentBytesRead < totalExpectedSize) {
				int r = recv(sock, (char*)fullPacketBuffer.data() + currentBytesRead, totalExpectedSize - currentBytesRead, 0);
				if (r <= 0)
				{
					logging("Server disconnected");
					terminate_connection(sock);
					break;
				};
				currentBytesRead += r;
			}

			Packet inputPacket = Packet::Deserialize(fullPacketBuffer.data(), currentBytesRead, false);
			crow::response res;
			res.code = 200;
			res.set_header("Content-Type", "application/json");
			res.body = inputPacket.payloadString();
			logging("Received warranty history report data from TCP server and sent response to browser");
			terminate_connection(sock);
			return res;
		}
		catch (const std::exception& e) {
			terminate_connection(sock);
			return crow::response(400, "Error processing request: " + std::string(e.what()));
		}
	});

	// Start the server on port 8080
	app.port(8080).multithreaded().run();

	//copy http://localhost:8080 to search bar

	return 0;
}