#include "RawSql.h"

#include <memory>
#include <stdexcept>
#include <string>

namespace {

using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

Statement prepare(sqlite3* connection, const char* sql) {
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(connection, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(connection));
    }
    return Statement(statement, sqlite3_finalize);
}

void check(sqlite3* connection, int result) {
    if (result != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(connection));
    }
}

}  // namespace

int rawInsertUser(sqlite3* connection, const User& user) {
    auto statement = prepare(
        connection,
        "INSERT INTO Users(name, department, status) VALUES (?, ?, ?)");

    check(connection, sqlite3_bind_text(statement.get(), 1, user.name.c_str(), -1, SQLITE_TRANSIENT));
    check(connection, sqlite3_bind_text(statement.get(), 2, user.department.c_str(), -1, SQLITE_TRANSIENT));
    check(connection, sqlite3_bind_text(statement.get(), 3, user.status.c_str(), -1, SQLITE_TRANSIENT));

    if (sqlite3_step(statement.get()) != SQLITE_DONE) {
        throw std::runtime_error(sqlite3_errmsg(connection));
    }
    return static_cast<int>(sqlite3_last_insert_rowid(connection));
}

std::optional<User> rawFindUser(sqlite3* connection, int id) {
    auto statement = prepare(
        connection,
        "SELECT id, name, department, status FROM Users WHERE id = ?");
    check(connection, sqlite3_bind_int(statement.get(), 1, id));

    const int result = sqlite3_step(statement.get());
    if (result == SQLITE_DONE) {
        return std::nullopt;
    }
    if (result != SQLITE_ROW) {
        throw std::runtime_error(sqlite3_errmsg(connection));
    }

    return User{
        sqlite3_column_int(statement.get(), 0),
        reinterpret_cast<const char*>(sqlite3_column_text(statement.get(), 1)),
        reinterpret_cast<const char*>(sqlite3_column_text(statement.get(), 2)),
        reinterpret_cast<const char*>(sqlite3_column_text(statement.get(), 3))};
}
