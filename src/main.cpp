#include "Repository.h"
#include "RawSql.h"

#include <sqlite3.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace sqlite_orm;

namespace {

void demonstrateRawSql() {
    sqlite3* connection = nullptr;
    if (sqlite3_open(":memory:", &connection) != SQLITE_OK) {
        const std::string message = connection ? sqlite3_errmsg(connection) : "SQLite open failed";
        if (connection) {
            sqlite3_close(connection);
        }
        throw std::runtime_error(message);
    }

    try {
        const char* schema =
            "CREATE TABLE Users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT NOT NULL, "
            "department TEXT NOT NULL, "
            "status TEXT NOT NULL)";

        char* error = nullptr;
        if (sqlite3_exec(connection, schema, nullptr, nullptr, &error) != SQLITE_OK) {
            const std::string message = error ? error : "Failed to create Users table";
            sqlite3_free(error);
            throw std::runtime_error(message);
        }

        const int id = rawInsertUser(connection, {0, "Raw API", "Demo", "active"});
        const auto user = rawFindUser(connection, id);
        std::cout << "Raw SQLite SELECT: " << (user ? user->name : "not found") << '\n';
    } catch (...) {
        sqlite3_close(connection);
        throw;
    }

    sqlite3_close(connection);
}

void seedData(Storage& storage, IUserRepository& users, DocumentRepository& documents) {
    for (int i = 1; i <= 5; ++i) {
        users.add({0, "User " + std::to_string(i), i % 2 ? "Office" : "IT", "active"});
    }

    for (const auto& user : users.all()) {
        for (int number = 1; number <= 3; ++number) {
            const int pages = number * 3 + user.id;
            documents.add({0,
                           user.id,
                           "Document " + std::to_string(number),
                           pages,
                           number % 2 ? "queued" : "printed",
                           pages * 0.5});
        }
    }

    std::cout << "Seed: users=" << storage.count<User>()
              << ", documents=" << storage.count<PrintDocument>() << '\n';
}

void demonstrateCrud(IUserRepository& users) {
    const int id = users.add({0, "ORM API", "Demo", "active"});
    auto user = users.find(id);
    if (!user) {
        throw std::runtime_error("ORM insert/find failed");
    }
    std::cout << "ORM INSERT/SELECT: " << user->name << '\n';

    user->department = "Updated";
    users.update(*user);
    const auto updated = users.find(id);
    std::cout << "ORM UPDATE: " << (updated ? updated->department : "not found") << '\n';

    users.remove(id);
    std::cout << "ORM DELETE: " << (!users.find(id) ? "OK" : "FAILED") << '\n';
}

void demonstrateQueries(Storage& storage) {
    const auto office = storage.get_all<User>(where(c(&User::department) == "Office"));
    std::cout << "Filter Office: " << office.size() << '\n';

    const auto sorted = storage.get_all<User>(order_by(&User::name), limit(3));
    std::cout << "Sorted top 3:";
    for (const auto& user : sorted) {
        std::cout << ' ' << user.name;
    }
    std::cout << '\n';

    const auto pages = storage.sum(&PrintDocument::pages);
    std::cout << "SUM pages: " << (pages ? *pages : 0) << '\n';
    std::cout << "COUNT documents: " << storage.count<PrintDocument>() << '\n';

    const auto joined = storage.select(
        columns(&User::name, &PrintDocument::title, &PrintDocument::pages),
        inner_join<PrintDocument>(on(c(&User::id) == &PrintDocument::userId)),
        limit(4));

    std::cout << "JOIN sample: " << joined.size() << " rows\n";
    for (const auto& row : joined) {
        std::cout << "  " << std::get<0>(row) << " -> " << std::get<1>(row)
                  << " (" << std::get<2>(row) << " pages)\n";
    }
}

void demonstrateTransactions(Storage& storage, IUserRepository& users,
                             DocumentRepository& documents) {
    const auto beforeUsers = storage.count<User>();
    const auto beforeDocuments = storage.count<PrintDocument>();

    const bool committed = storage.transaction([&] {
        const int id = users.add({0, "Transaction OK", "IT", "active"});
        documents.add({0, id, "New print job", 10, "queued", 5.0});
        return true;
    });

    if (!committed || storage.count<User>() != beforeUsers + 1 ||
        storage.count<PrintDocument>() != beforeDocuments + 1) {
        throw std::runtime_error("Commit verification failed");
    }
    std::cout << "Commit verified: YES\n";

    const auto usersBeforeRollback = storage.count<User>();
    const auto docsBeforeRollback = storage.count<PrintDocument>();
    bool failedAsExpected = false;

    try {
        storage.transaction([&] {
            users.add({0, "Rollback user", "IT", "active"});
            documents.add({0, -999, "Invalid foreign key", 3, "queued", 1.0});
            return true;
        });
    } catch (const std::exception& error) {
        failedAsExpected = true;
        std::cout << "Expected transaction failure: " << error.what() << '\n';
    }

    const bool rolledBack = failedAsExpected &&
                            storage.count<User>() == usersBeforeRollback &&
                            storage.count<PrintDocument>() == docsBeforeRollback;
    std::cout << "Rollback verified: " << (rolledBack ? "YES" : "NO") << '\n';
    if (!rolledBack) {
        throw std::runtime_error("Rollback verification failed");
    }
}

}  // namespace

int main() {
    try {
        auto storage = createStorage(":memory:");
        storage.on_open = [](sqlite3* connection) {
            char* error = nullptr;
            if (sqlite3_exec(connection, "PRAGMA foreign_keys = ON", nullptr, nullptr, &error) != SQLITE_OK) {
                const std::string message = error ? error : "Cannot enable foreign keys";
                sqlite3_free(error);
                throw std::runtime_error(message);
            }
        };
        storage.sync_schema();

        UserRepository concreteUsers(storage);
        IUserRepository& users = concreteUsers;
        DocumentRepository documents(storage);

        std::cout << "LAB 8 / VARIANT 6 / PRINT QUEUE\n";
        seedData(storage, users, documents);
        demonstrateRawSql();
        demonstrateCrud(users);
        demonstrateQueries(storage);
        demonstrateTransactions(storage, users, documents);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
