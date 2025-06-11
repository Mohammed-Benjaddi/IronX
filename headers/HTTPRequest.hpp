#pragma once
#include "IHTTPMessage.hpp"
#include "WebServerConfig.hpp"
#include "FileStreamer.hpp"
#include "HTTPResponse.hpp"
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
// #include "CGI.hpp"

class HTTPResponse;
class FileStreamer;

typedef struct sFormFile {
    std::string name;
    std::string filename;
    std::string contentType;
    std::vector<char> data;
} FormFile;

class HTTPRequest : public IHTTPMessage {
private:
    WebServerConfig *config;
    int clientId;
    HTTPResponse *response;
    FileStreamer *streamer;
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string query;
    std::string fileContent;
    bool bodyFound;
    std::string rootDir;
    std::vector<FormFile> formFiles;
    std::string fileExtension;
    std::string location;
    std::map<int, std::string> error_pages;

public:
    HTTPRequest(const std::string &, WebServerConfig *, int );
    HTTPRequest(const std::string &, WebServerConfig *, int , HTTPResponse *);
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

    void setBodyFound(bool b);
    void setFormFile(std::vector<FormFile>& formFiles);
    void setRootDir(std::string rootDor);
    void setFileExtension(const std::string& path);
    void setLocation(std::string& location);
    void setErrorPages(const std::map<int, std::string>& error_pages);

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
    std::string getRootDir() const;
    std::string getLocation() const;
    std::string getErrorPages(int code) const;
    std::string getBoundary() const;
    bool isBodyFound() const;
    std::vector<FormFile> &getFormFiles();
    std::string getFileExtension();

    virtual std::vector<uint8_t> to_bytes() const;

    int setRoutesInfo(std::map<std::string, Route> &routes, Route &route);

    void handleRequest();
    void handleGet(std::map<std::string, Route> &routes, Route &route);
    void handlePOST();
    void handleDELETE(std::map<std::string, Route> &routes, Route &route);

    void executeCGI(Route &route);
};