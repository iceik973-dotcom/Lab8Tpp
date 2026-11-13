#pragma once

#include "Database.h"

#include <optional>
#include <vector>

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual int add(const User& user) = 0;
    virtual std::optional<User> find(int id) = 0;
    virtual std::vector<User> all() = 0;
    virtual void update(const User& user) = 0;
    virtual void remove(int id) = 0;
};

class UserRepository final : public IUserRepository {
public:
    explicit UserRepository(Storage& storage) : storage_(storage) {}

    int add(const User& user) override { return storage_.insert(user); }
    std::optional<User> find(int id) override { return storage_.get_optional<User>(id); }
    std::vector<User> all() override { return storage_.get_all<User>(); }
    void update(const User& user) override { storage_.update(user); }
    void remove(int id) override { storage_.remove<User>(id); }

private:
    Storage& storage_;
};

class DocumentRepository {
public:
    explicit DocumentRepository(Storage& storage) : storage_(storage) {}

    int add(const PrintDocument& document) { return storage_.insert(document); }
    std::vector<PrintDocument> all() { return storage_.get_all<PrintDocument>(); }

private:
    Storage& storage_;
};
