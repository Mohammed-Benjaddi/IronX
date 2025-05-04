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

class HTTPRequest : public IHTTPMessage {
private:
    WebServerConfig &config;
    std::string method;                 // "GET", "POST", etc.
    std::string path;                   // "/index.html"
    std::string httpVersion;            // "HTTP/1.1"
    std::map<std::string, std::string> headers; // Key-value headers
    std::string body;                   // Request payload (for POST/PUT)
    std::string query;                  // Query string (if any)
    int clientId;
    std::string fileContent;
public:
    // HTTPRequest();
    HTTPRequest(std::fstream &file, WebServerConfig &config, int clientId);
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
    WebServerConfig getConfig() const;
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
    // int parse(std::fstream &file);
    // void find_method_uri(const std::string &line);
    // std::string findHeader(std::string key);
    // bool checkRequestURI(std::string);
    // bool URIHasUnallowedChar(std::string uri);
    // int findLocation();
    // int checkAllowedMethods();

    // void pathIsDirectory(std::map<std::string, Route> &routes, Route &route, const std::string &_path);
    // void directoryHasNoIndexFiles(Route &route);
    // void directoryHasIndexFiles(Route &route, std::vector<std::string> index_files);

    // void pathIsFile(std::map<std::string, Route> &routes, Route &route);
    // void fileHasNoCGI(Route &route, std::string &filename);

    // void autoIndexOfDirectory(Route &route);
    // std::vector<std::string> getDirectoryListing(const std::string& path, bool show_hidden);

    void handleRequest();
    void handleGet();
    void handlePOST();
    void handleDELETE();

    void executeCGI();
};
