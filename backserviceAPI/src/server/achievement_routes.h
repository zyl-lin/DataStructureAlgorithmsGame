#pragma once

#include "crow.h"
#include "database.h"

void registerAchievementRoutes(crow::App<crow::CORSHandler>& app, Database& db); 