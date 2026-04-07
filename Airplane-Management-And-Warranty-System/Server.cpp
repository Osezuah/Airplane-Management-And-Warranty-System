#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include <libpq-fe.h>
#include "..\Airplane-Management-And-Warranty-System\Packet.h"
#include "..\Airplane-Management-And-Warranty-System\PacketFactory.h"

void send_all_airplanes_to_client(SOCKET clientSocket, PGconn* conn, Packet request) {
	// Get Header
	/*std::vector<uint8_t> hBuf(PACKETHEADER_BYTE_SIZE);
	int hRec = recv(clientSocket, (char*)hBuf.data(), PACKETHEADER_BYTE_SIZE, 0);*/

	/*if (hRec == PACKETHEADER_BYTE_SIZE) {
		Packet reqHead = Packet::Deserialize(hBuf.data(), hRec);*/

		// If there's a payload, get it (though QUERY_REQUEST is usually just header)
		/*if (request.getType() == PacketType::QUERY_REQUEST) {*/

		// Database logic
		PGresult* res = PQexec(conn, "SELECT row_to_json(t) FROM (SELECT * FROM airplanes) t;");
		int rows = PQntuples(res);

		std::string records = "[";
		for (int i = 0; i < rows; i++) {
			records += PQgetvalue(res, i, 0);
			if (i < rows - 1) records += ",";
		}
		records += "]";
		PQclear(res);

		// Send the response back
		Packet response = PacketFactory::QueryResponse(request.getSequence(), rows, records);
		std::vector<uint8_t> out = response.Serialize();
		send(clientSocket, (const char*)out.data(), (int)out.size(), 0);
		/*}*/
	/*}*/
}

int main() {
	// Connect to the PostgreSQL database on docker v17.4
	//Airplane Management and Warranty System Server
	// Connect to the PostgreSQL database v17.4
	PGconn* conn = PQconnectdb(
		"host=127.0.0.1 port=5432 dbname=amws user=postgres password=Lkj876*bv"
	);
	if (PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		return 1;
	}
	else {

		//TCP Server
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);

		int wsOk = WSAStartup(ver, &wsData);
		if (wsOk != 0) {
			std::cerr << "Can't initialize winsock!" << std::endl;
			return 1;
		}

		SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in hint{};
		hint.sin_family = AF_INET;
		hint.sin_port = htons(54000);
		hint.sin_addr.S_un.S_addr = INADDR_ANY;

		if (bind(serverSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
			std::cout << "Bind failed! Error: " << WSAGetLastError() << std::endl;
			return 1;
		}

		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
			std::cout << "Listen failed! Error: " << WSAGetLastError() << std::endl;
			return 1;
		}
		
		//loop to keep the server constantly running
		while (true) {
			SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
			if (clientSocket == INVALID_SOCKET)
			{
				continue;
			}

			uint8_t buf[PACKETHEADER_BYTE_SIZE];
			int bytesReceived = recv(clientSocket, (char*)buf, PACKETHEADER_BYTE_SIZE, 0);

			if (bytesReceived >= (int)PACKETHEADER_BYTE_SIZE) {
				Packet recvPacket = Packet::Deserialize(buf, bytesReceived, true);
				PacketType type = recvPacket.getType();

				switch (type) {
				case PacketType::HANDSHAKE:
				{
					std::string response = "Connected";
					send(clientSocket, response.c_str(), (int)response.size(), 0);
					std::cout << "Handshake successful." << std::endl;
					break;
				}

				case PacketType::QUERY_REQUEST:
				{
					std::cout << "Querying DB..." << std::endl;
					send_all_airplanes_to_client(clientSocket, conn, recvPacket);
					break;
				}

				default:
				{
					std::cout << "Unknown Packet Type received: " << (int)type << std::endl;
					break;
				}
				}
			}
			closesocket(clientSocket);
		}

	}
	return 0;
}