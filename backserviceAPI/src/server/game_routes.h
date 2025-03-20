#pragma once

#include "crow.h"
#include "database.h"
#include <crow/middlewares/cors.h>

void registerGameRoutes(crow::App<crow::CORSHandler>& app, Database& db); 