#pragma once

#include "Models.h"

#include <sqlite_orm/sqlite_orm.h>

#include <string>

inline auto createStorage(const std::string& path) {
    using namespace sqlite_orm;

    return make_storage(
        path,
        make_table(
            "Users",
            make_column("id", &User::id, primary_key().autoincrement()),
            make_column("name", &User::name, not_null()),
            make_column("department", &User::department, not_null()),
            make_column("status", &User::status, not_null())),
        make_table(
            "PrintDocuments",
            make_column("id", &PrintDocument::id, primary_key().autoincrement()),
            make_column("user_id", &PrintDocument::userId),
            make_column("title", &PrintDocument::title, not_null()),
            make_column("pages", &PrintDocument::pages),
            make_column("status", &PrintDocument::status, not_null()),
            make_column("cost", &PrintDocument::cost),
            foreign_key(&PrintDocument::userId).references(&User::id)));
}

using Storage = decltype(createStorage(""));
