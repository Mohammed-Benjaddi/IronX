#pragma once

#include <ctime>
#include <time.h>
#include <string>
#include <sstream>


class Cookie {
private:
    std::string _header; 
    std::string _headerName;
    std::string _headerValue;
public:
    Cookie(const std::string &name, const std::string &value, int maxAge);
    std::string getHeader() const;
    void setHeader(const std::string &header) {
        _header = header;
    };
};