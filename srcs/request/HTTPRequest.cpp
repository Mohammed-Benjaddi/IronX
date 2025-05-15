#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::fstream &file, WebServerConfig &_config, int _clientId) : IHTTPMessage(), config(_config), clientId(_clientId) {
    parse(*this, file);
    checkAllowedMethods(*this);
    handleRequest();
}

HTTPRequest::~HTTPRequest() {}

void HTTPRequest::setMethod(const std::string &method) {
    this->method = method;
}

void HTTPRequest::setHeaders(std::string line) {
    size_t pos = line.find(':');
    if (pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        setHeader(key, value);
    }
}

void HTTPRequest::setPath(const std::string &path) {
    this->path = path;
}

void HTTPRequest::setHTTPVersion(const std::string &version) {
    this->httpVersion = version;
}

void HTTPRequest::setHeader(const std::string &key, const std::string &value) {
    headers[key] = value;
}

void HTTPRequest::setBody(const std::string &body) {
    this->body = body;
}

void HTTPRequest::setQuery(const std::string &query) {
    this->query = query;
}

void HTTPRequest::setFileContent(const std::string &fileContent) {
    this->fileContent = fileContent;
}

void HTTPRequest::setClientId(int _clientId) {
    this->clientId = _clientId;
}

WebServerConfig &HTTPRequest::getConfig() const {
    return config;
}

std::string HTTPRequest::getMethod() const {
    return method;
}

std::string HTTPRequest::getPath() const {
    return path;
}

std::string HTTPRequest::getHTTPVersion() const {
    return httpVersion;
}

std::string HTTPRequest::getHeader(const std::string &key) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end()) {
        return it->second;
    }
    return "";
}

std::string HTTPRequest::getBody() const {
    return body;
}

std::map<std::string, std::string> HTTPRequest::getHeaders() const {
    return headers;
}

std::string HTTPRequest::getHeaderValue(const std::string &key) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end())
    {
        return it->second;
    }
    return "";
}

std::string HTTPRequest::getQuery() const {
    return query;
}

std::string HTTPRequest::getFileContent() const {
    return fileContent;
}

int HTTPRequest::getClientId() const {
    return clientId;
}

// Methods

std::vector<uint8_t> HTTPRequest::to_bytes() const {
    std::vector<uint8_t> bytes;
    return bytes;
}

void HTTPRequest::handleRequest() {
    if(getMethod() == "GET")
        handleGet();
    else if(getMethod() == "POST")
        handlePOST();
    else if(getMethod() == "DELETE")
        handleDELETE();
}

void HTTPRequest::executeCGI(Route &route) {
    if(getMethod() == "DELETE") {
        std::cout << "a file must be deleted" << std::endl;
        deleteRequestedFile(*this, "/" + route.getRootDir() + getPath(), "");
    }
}

void HTTPRequest::handleGet() {
    std::map<std::string, Route> routes = config.getClusters()[clientId].getRoutes();

    std::map<std::string, Route>::const_iterator it_route = routes.find(getPath());
    std::string pathToSearch;
    Route route;
    std::cout << "GET method" << std::endl;
    if(it_route == routes.end()) {
        route.setRootDir("home/simo/cursus/webserv/www/html");
        route.setAutoindex(false);
    } else
        copyToRoute(route, it_route);
    if(isDirExist(getPath(), route.getRootDir())) {
        std::cout << "-------> it is a directoty" << std::endl;
        pathIsDirectory(*this, routes, route, getPath());
    }
    else
        pathIsFile(*this, routes, route);
}

void HTTPRequest::handlePOST() {
    std::cout << "POST method" << std::endl;
}

void HTTPRequest::handleDELETE() {
    // common funcs
    // isDirectory
    // location has CGI
    // does the directory have index files

    /*
        **** notes ****
        if location has a redirection, the one that must be deleted is the redirected location
    */
    std::cout << "DELETE method" << std::endl;
    std::map<std::string, Route> routes = config.getClusters()[clientId].getRoutes();

    std::map<std::string, Route>::const_iterator it_route = routes.find(getPath());
    std::cout << "* it route ---> " << getPath() << std::endl;
    std::string pathToSearch;
    Route route;
    if(it_route == routes.end()) {
        route.setRootDir("home/simo/cursus/webserv/www");
        route.setAutoindex(false);
    } else
        copyToRoute(route, it_route);
    if(isDirExist(getPath(), route.getRootDir())) {
        std::cout << "path is directory" << std::endl;
        DELETEDirectory(*this, routes, route, getPath());
    }
    else
        pathIsFile(*this, routes, route);
}
