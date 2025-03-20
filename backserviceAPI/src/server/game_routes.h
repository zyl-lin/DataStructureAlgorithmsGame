#pragma once

#include "crow.h"
#include "database.h"

void registerGameRoutes(crow::App& app, Database& db); 