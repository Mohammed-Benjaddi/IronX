#pragma once
#include "IHTTPMessage.hpp"
#include "WebServerConfig.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include <set>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "requestUtils.hpp"
#include "parseRequest.hpp"
#include <filesystem>
#include <cstring>

class HTTPRequest : public IHTTPMessage {
private:
    WebServerConfig *config;
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string query;
    int clientId;
    std::string fileContent;
public:
    HTTPRequest(const std::string &raw_request, WebServerConfig *config, int clientId);
    ~HTTPRequest();

    // Setters
    void setMethod(const std::string& method);
    void setPath(const std::string& path);
    void setHTTPVersion(const std::string& version);
    void setHeader(const std::string& key, const std::string& value);
    void setHeaders(std::string line);
    void setBody(const std::string& body);
    void setQuery(const std::string& query);
    void setFileContent(const std::string& fileContent);
    void setClientId(int _clientId);

    // Getters
    WebServerConfig *getConfig() const;
    std::string getMethod() const;
    std::string getPath() const;
    std::string getHTTPVersion() const;
    std::string getHeader(const std::string& key) const;
    std::string getBody() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getHeaderValue(const std::string& key) const;
    std::string getQuery() const;
    std::string getFileContent() const;
    int getClientId() const;

    virtual std::vector<uint8_t> to_bytes() const;

    void handleRequest();
    void handleGet();
    void handlePOST();
    void handleDELETE();

    void executeCGI(Route &route);
};
