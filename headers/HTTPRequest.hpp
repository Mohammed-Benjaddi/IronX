#pragma once
#include "IHTTPMessage.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

class HTTPRequest : public IHTTPMessage {
    private:
    std::string method;                 // "GET", "POST", etc.
    std::string path;                   // "/index.html"
    std::string httpVersion;            // "HTTP/1.1"
    std::unordered_map<std::string, std::string> headers; // Key-value headers
    std::string body;                   // Request payload (for POST/PUT)
    // Response Attributes
    int statusCode;                     // 200, 404, 500
    std::string statusMessage;          // "OK", "Not Found"
    // std::unordered_map<std::string, std::string> responseHeaders;
    // std::string response;           // Data to send back
    // int clientSocket;
public:
    // Constructors
    HTTPRequest() {};
    // HTTPRequest(const std::vector<uint8_t>& raw_data);
    // HTTPRequest(const std::string& method,
    //             const std::string& path,
    //             const std::map<std::string, std::string>& headers,
    //             const std::vector<uint8_t>& body);

    // Build from file 
    static HTTPRequest from_file(const std::string& path);

    // Implement virtual functions
    virtual std::vector<uint8_t> to_bytes() const;

    // Request data
    // std::string method;
    // std::string path;
    // std::string query;

    // // Request processing
    // bool parse(const std::vector<uint8_t>& raw_data);
    // bool is_body_too_large(size_t max_size) const;

// private:
//     void parse_query_string();
};