#pragma once

#include "crow.h"
#include "database.h"

// 不使用 CORSHandler，直接使用普通的 App
void registerUserRoutes(crow::App& app, Database& db); 