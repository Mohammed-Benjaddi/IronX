#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include <iterator>

typedef unsigned char uint8_t; 

class IHTTPMessage {

protected:
    std::string version;
    std::map<std::string, std::string> headers;
    int statusCode;                     // 200, 404, 500
    std::string statusMessage;          // "OK", "Not Found"
public:
    virtual ~IHTTPMessage() {}


    void setStatusMessage(const std::string& message);
    void setStatusCode(int code);
    std::string getStatusMessage() const;
    int getStatusCode() const;

    std::vector<uint8_t> body;
};