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
    // std::vector<uint8_t> body;

public:
    IHTTPMessage();
    virtual ~IHTTPMessage() {}
    // Header operations
    bool has_header(const std::string& name) const;
    std::string get_header(const std::string& name) const;
    void set_header(const std::string& name, const std::string& value);

    // Body and connection info
    size_t content_length() const;
    bool keep_alive() const;

    // Serialize to raw bytes
    virtual std::vector<uint8_t> to_bytes() const = 0;

    std::vector<uint8_t> body;
};