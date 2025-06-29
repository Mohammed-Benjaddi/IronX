#include "../../headers/Cookie.hpp"

Cookie::Cookie(const std::string &name, const std::string &value, int maxAge) {
    time_t now = time(NULL);
    time_t expire_time = now + maxAge;

    char expires_str[100];
    strftime(expires_str, sizeof(expires_str), "%a, %d-%b-%Y %H:%M:%S GMT", gmtime(&expire_time));

    std::ostringstream oss;
    oss << name << "=" << value << "; ";
    oss << "Max-Age=" << maxAge << "; ";
    oss << "Expires=" << expires_str << "; ";
    oss << "Path=/";
    _header = oss.str();
}

std::string Cookie::getHeader() const {
    return _header;
}