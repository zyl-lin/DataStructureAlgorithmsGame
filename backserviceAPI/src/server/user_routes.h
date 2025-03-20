#pragma once

#include "crow.h"
#include "database.h"

void registerUserRoutes(crow::App& app, Database& db); 