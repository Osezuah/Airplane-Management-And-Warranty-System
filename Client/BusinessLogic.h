#pragma once
#include <libpq-fe.h>
#include "asio.hpp"
#include "crow.h"

crow::json::wvalue get_airplane_data_from_db(PGconn* conn);