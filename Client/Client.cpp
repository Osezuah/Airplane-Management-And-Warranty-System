#include "asio.hpp"
#include "crow.h"
#include <libpq-fe.h>
#include <winsock.h>
#include "..\Airplane-Management-And-Warranty-System\Packet.h"
#include "..\Airplane-Management-And-Warranty-System\PacketFactory.h"

//function to connect to serve via TCP sockets by sending a message - client
std::string send_to_tcp_server(const std::string& message, SOCKET sock) {
	send(sock, message.c_str(), message.size(), 0);

	char buffer[PACKETHEADER_BYTE_SIZE];
	int bytesReceived = recv(sock, buffer, PACKETHEADER_BYTE_SIZE, 0);

	return std::string(buffer, bytesReceived);
}

std::string get_all_airplanes_from_server(SOCKET sock) {
	// Create and Serialize Request
	Packet request = PacketFactory::QueryRequest(1, 0);
	std::vector<uint8_t> serialized = request.Serialize();
	send(sock, (const char*)serialized.data(), (int)serialized.size(), 0);

	// Read the Header First
	std::vector<uint8_t> headerBuffer(PACKETHEADER_BYTE_SIZE);
	int hReceived = recv(sock, (char*)headerBuffer.data(), PACKETHEADER_BYTE_SIZE, 0);

	Packet tempHeader = Packet::Deserialize(headerBuffer.data(), hReceived, true);
	uint32_t payloadLen = tempHeader.header.payloadLength;

	// Read the Payload
	std::vector<uint8_t> fullPacket = headerBuffer;
	if (payloadLen > 0) {
		std::vector<uint8_t> payloadBuffer(payloadLen);
		int pReceived = 0;
		while (pReceived < (int)payloadLen) {
			int r = recv(sock, (char*)payloadBuffer.data() + pReceived, payloadLen - pReceived, 0);
			if (r <= 0) break;
			pReceived += r;
		}
		// Add the payload to the end of the header bytes
		fullPacket.insert(fullPacket.end(), payloadBuffer.begin(), payloadBuffer.end());
	}

	Packet finalPacket = Packet::Deserialize(fullPacket.data(), fullPacket.size(), false);
	return finalPacket.payloadString();
}

int main() {
	//SPRINT # 1 
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
	CROW_ROUTE(app, "/connect-to-tcp-server").methods("POST"_method)
		([](const crow::request& req) {
			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in serverAddr{};
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(54000);
			inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

			connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
		
			Packet handshake = PacketFactory::Handshake(1, "client", "1111"); 
			std::vector<uint8_t> out = handshake.Serialize();

			send(sock, (const char*)out.data(), (int)out.size(), 0);

			char buffer[PACKETHEADER_BYTE_SIZE];
			int bytes = recv(sock, buffer, PACKETHEADER_BYTE_SIZE, 0);
			std::string tcpResponse(buffer, bytes);
			closesocket(sock);
			return crow::response("{\"status\": \"" + tcpResponse + "\"}");
		});

	//route to access airplanes in database
	CROW_ROUTE(app, "/list/airplane").methods("GET"_method)
		([]() {
			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in serverAddr{};
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(54000);
			inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

			connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

			std::string airplanesJson = get_all_airplanes_from_server(sock);
			closesocket(sock);
			return crow::response(airplanesJson);
		});

	// Start the server on port 8080
	app.port(8080).multithreaded().run();

	//copy http://localhost:8080 to search bar

	return 0;
}