#include "asio.hpp"
#include "crow.h"
#include <libpq-fe.h>
#include <winsock.h>

using namespace std;

//function to connect to serve via TCP sockets by sending a message - client
std::string send_to_tcp_server(const std::string& message) {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(54000);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

	send(sock, message.c_str(), message.size(), 0);

	char buffer[4096];
	int bytesReceived = recv(sock, buffer, 4096, 0);

	return std::string(buffer, bytesReceived);
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
		//return "Welcome to the Airplane Management and Warranty System!";
		});

	//route to connect to server via TCP sockets
	CROW_ROUTE(app, "/connect-to-tcp-server").methods("POST"_method)
		([](const crow::request& req) {

			std::string requestData = req.body;

			// Send to TCP server
			std::string response = send_to_tcp_server(requestData);

			return crow::response(response);
		});

	// Start the server on port 8080
	app.port(8080).multithreaded().run();

	//copy http://localhost:8080 to search bar

	return 0;
}