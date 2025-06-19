#pragma once

#include <ctime>
#include <time.h>
#include <string>
#include <sstream>


class Cookie {
private:
    std::string _header;

public:
    Cookie(const std::string &name, const std::string &value, int maxAge);

    std::string getHeader() const;
};