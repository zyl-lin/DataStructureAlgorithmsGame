#pragma once

#include "crow.h"
#include "database.h"

void registerGameRoutes(crow::App<crow::CORSHandler>& app, Database& db); 