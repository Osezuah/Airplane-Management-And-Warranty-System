#include "SocketConnection.h"

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