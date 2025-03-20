#pragma once

#include "crow.h"
#include "database.h"

void registerUserRoutes(crow::App<crow::CORSHandler>& app, Database& db); 