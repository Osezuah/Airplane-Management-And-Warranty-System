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

			if (bytesReceived <= 0)
		}
		closesocket(clientSocket);
	}
	closesocket(serverSocket);
	WSACleanup();
	PQfinish(conn);
	return 0;
}