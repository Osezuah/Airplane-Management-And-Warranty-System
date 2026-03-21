#include "asio.hpp"
#include "crow.h"
#include <libpq-fe.h>


int main() {
	//AI generated code by ChatGPT-4, 2024-06-01
	//Airplane Management and Warranty System Server
	// Connect to the PostgreSQL database v17.4
	PGconn* conn = PQconnectdb(
		"host=localhost port=5432 dbname=amws user=postgres password=Lkj876*bv"
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
	// Define a route for the root URL
	CROW_ROUTE(app, "/")([]() {
		return "Welcome to the Airplane Management and Warranty System!";
		});
	// Start the server on port 8080
	app.port(8080).multithreaded().run();

	//search http://localhost:8080 to see the welcome message
    
	return 0;
}