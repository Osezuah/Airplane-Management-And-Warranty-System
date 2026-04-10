#include "asio.hpp"
#include "crow.h"
#include <libpq-fe.h>
#include <winsock.h>
#include "..\Airplane-Management-And-Warranty-System\Packet.h"
#include "..\Airplane-Management-And-Warranty-System\PacketFactory.h"

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

std::string get_all_airplanes_from_server(SOCKET sock) {
	// Create and Serialize Request
	Packet request = PacketFactory::QueryRequest(1, 0);
	std::vector<uint8_t> serialized = request.Serialize();
	send(sock, (const char*)serialized.data(), (int)serialized.size(), 0);

	// Read the Header First
	std::vector<uint8_t> headerBuffer(PAGE_SIZE);
	int hReceived = recv(sock, (char*)headerBuffer.data(), PAGE_SIZE, 0);

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

			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in serverAddr{};
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(27000);
			inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

			connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
			
			//recv
			//std::vector<uint8_t> rxBuffer(PAGE_SIZE);
			//outputPacket = PacketFactory::HandshakeAck(inputPacket.getSequence(), true, "SESSION_VALID");
			//send(clientSocket, (char*)txData.data(), txData.size(), 0);
			
			//send
			/*Packet handshakePacket = PacketFactory::Handshake(1, "1", "Client");
			std::vector<uint8_t> txData = handshakePacket.Serialize();
			send(sock, (char*)txData.data(), txData.size(), 0);*/

			handshake_with_tcp_server(sock);
			closesocket(sock);
			return "";
	});

	//route to access airplanes in database
	CROW_ROUTE(app, "/list/airplane")([]() {
			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in serverAddr{};
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(27000);
			inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

			connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

			handshake_with_tcp_server(sock);

			//std::vector<uint8_t> rxBuffer(PAGE_SIZE);
			//outputPacket = PacketFactory::HandshakeAck(inputPacket.getSequence(), true, "SESSION_VALID");
			//send(clientSocket, (char*)txData.data(), txData.size(), 0);
			
			closesocket(sock);
		//return crow::response(airplanesJson);
			return "";
		});

	// Start the server on port 8080
	app.port(8080).multithreaded().run();

	//copy http://localhost:8080 to search bar

	return 0;
}