#pragma once

#include <string>

struct User {
    int id = 0;
    std::string name;
    std::string department;
    std::string status;
};

struct PrintDocument {
    int id = 0;
    int userId = 0;
    std::string title;
    int pages = 0;
    std::string status;
    double cost = 0.0;
};
