#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>

using namespace std;

int main() {
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

	bind(serverSocket, (sockaddr*)&hint, sizeof(hint));

	//loop to keep the server constantly running
	while (1) {
		listen(serverSocket, SOMAXCONN); 

		SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

		char buf[4096];
		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		std::string response = "Processed: " + std::string(buf, bytesReceived);

		send(clientSocket, response.c_str(), response.size(), 0);

		cout << response;
	}	

	return 0;
}