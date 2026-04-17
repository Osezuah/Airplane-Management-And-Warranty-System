#include "BusinessLogic.h"
// functions to handle business logic for the client, such as connecting to the DB, TCP server and sending packets.

crow::json::wvalue get_airplane_data_from_db(PGconn* conn)
{
	const char* command = "SELECT * FROM Airplane";
	PGresult* result = PQexec(conn, command);

	if (PQresultStatus(result) != PGRES_TUPLES_OK) {
		PQclear(result);
		throw std::runtime_error("Database Query Failed");
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

	return finalResponse;
}
