#pragma once

#include "crow.h"
#include "database.h"
#include <crow/middlewares/cors.h>

void registerAchievementRoutes(crow::App<crow::CORSHandler>& app, Database& db); 