#pragma once

#include "Models.h"

#include <sqlite3.h>

#include <optional>

int rawInsertUser(sqlite3* connection, const User& user);
std::optional<User> rawFindUser(sqlite3* connection, int id);
