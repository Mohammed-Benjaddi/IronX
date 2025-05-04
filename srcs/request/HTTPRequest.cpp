#include "HTTPRequest.hpp"

const std::string HTTPRequest::chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_-~/?#[]@!$$('*+,'=%.";
const std::vector<char> HTTPRequest::allowedChars(chars.begin(), chars.end());

HTTPRequest::HTTPRequest(std::fstream &file, WebServerConfig &_config, int _clientId) : IHTTPMessage(), config(_config), clientId(_clientId) {
    parse(file);
    checkAllowedMethods();
    handleRequest();
}

HTTPRequest::~HTTPRequest() {}

void HTTPRequest::setMethod(const std::string &method) {
    this->method = method;
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

// Methods
bool HTTPRequest::URIHasUnallowedChar(std::string uri) {
    for(size_t i = 0; i < uri.size(); i++) {
        if(std::find(allowedChars.begin(), allowedChars.end(), uri[i]) == allowedChars.end()) {
            std::cout << "unallowed char ---> " << uri[i] << std::endl;
            return true;
        }
    }
    return false;
}

bool HTTPRequest::checkRequestURI(std::string uri) {
    // 400 Bad request
    if(URIHasUnallowedChar(uri)) {
        setStatusCode(400);
        setStatusMessage("Bad request");
        return false;
    }
    // 414 Request-URI Too Long
    if(uri.length() > 2048) {
        setStatusCode(414);
        setStatusMessage("Request-URI Too Long");
        return false;
    }
    return true;
}

void HTTPRequest::find_method_uri(const std::string &line) {
    std::stringstream sstream(line);
    std::string method, uri, httpVersion;
    sstream >> method >> uri >> httpVersion;
    if(!checkRequestURI(uri)) {
        std::cout << "URI is not correct" << std::endl;
        return;
    }
    // Check if the uri contains a query string
    size_t queryPos = uri.find('?');
    if (queryPos != std::string::npos) {
        setQuery(uri.substr(queryPos + 1));
        setPath(uri.substr(0, queryPos));
    }
    setMethod(method);
    setPath(uri);
    setHTTPVersion(httpVersion);
}

void HTTPRequest::setHeaders(std::string line) {
    size_t pos = line.find(':');
    if (pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        setHeader(key, value);
    }
}

int HTTPRequest::parse(std::fstream &file) {
    std::string line;
    std::stringstream ss;

    ss << file.rdbuf();
    // 413 error code if the request body larger than client max body in config file
    if(ss.str().length() > config.getMaxBodySize()) {
        setStatusCode(413);
        setStatusMessage("Request Entity Too Large");
        return -1;
    }
    std::getline(ss, line);
    find_method_uri(line);
    while (std::getline(ss, line) && line != "\r")
        setHeaders(line);
    // 501 Not implemented
    if (!findHeader("Transfer-Encoding").empty() && findHeader("Transfer-Encoding") != "chunked") {
        setStatusCode(501);
        setStatusMessage("Not Implemented");
        return -1;
    }
    return 1;
}

std::vector<uint8_t> HTTPRequest::to_bytes() const {
    std::vector<uint8_t> bytes;
    return bytes;
}

std::string HTTPRequest::findHeader(std::string key) {
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end())
        return it->second;
    return "";
}

int HTTPRequest::checkAllowedMethods() {
    const std::map<std::string, Route>& routes = config.getClusters()[clientId].getRoutes();
    std::map<std::string, Route>::const_iterator route_it = routes.find(this->getPath());
    const std::set<std::string> &allowedMethod = route_it->second.getAllowedMethods();

    if(allowedMethod.find(this->method) == allowedMethod.end()) {
        setStatusCode(405);
        setStatusMessage("Method Not Allowed");
        return -1; 
    }
    return 1;
}

void HTTPRequest::handleRequest() {
    if(getMethod() == "GET")
        handleGet();
    else if(getMethod() == "POST")
        handlePOST();
    else if(getMethod() == "DELETE")
        handleDELETE();
}

std::vector<std::string> HTTPRequest::getDirectoryListing(const std::string& path, bool show_hidden) {
    std::vector<std::string> entries;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string filename(ent->d_name);
            if (filename == "." || filename == "..") {
                continue;
            }
            if (!show_hidden && !filename.empty() && filename[0] == '.') {
                continue;
            }
            bool is_dir = (ent->d_type == DT_DIR);
            if (is_dir) {
                filename += "/";
            }
            entries.push_back(filename);
        }
        closedir(dir);
        std::sort(entries.begin(), entries.end());
    }
    return entries;
}

void HTTPRequest::autoIndexOfDirectory(Route &route) {
    // search for index.html
    std::cout << "start listing files" << std::endl;
    std::string indexes[2] = {"index.html", "index.htm"};
    std::string path = route.getRootDir() + "/";
    for(size_t i = 0; i < 2; i++) {
        if(isfileExist((path + indexes[i]).c_str())) {
            std::cout << "file ----> " << path + indexes[i] << std::endl;
            return;
        }
    }
    std::vector<std::string> entries = getDirectoryListing(path, false);
    std::cout << "route ===> " << route.getRootDir() << std::endl;
    for(size_t i = 0; i < entries.size(); i++) {
        std::cout << "---> " << entries[i] << std::endl;
    }
}

void HTTPRequest::directoryHasNoIndexFiles(Route &route) {
    if(!route.isAutoindex()) {
        // 403 Forbidden must be implemented here if autoindex is off
        setStatusCode(403);
        setStatusMessage("Forbidden");
    } else {
        autoIndexOfDirectory(route);
        // return autoindex of the directory
    }
}

void HTTPRequest::executeCGI() {

}


void HTTPRequest::directoryHasIndexFiles(Route &route, std::vector<std::string> index_files) {
    std::cout << "index files: " << std::endl;
    for(size_t i = 0; i < index_files.size(); i++) {
        std::cout << "index file #" << i << " --> " << route.getRootDir() + "/" + getPath() + "/" +  index_files[i] << std::endl;
        std::string path = "/" + route.getRootDir() + "/" + getPath() + "/" + index_files[i];
        if(isfileExist(path.c_str())) {
            std::cout << "I found the file" << std::endl;
            if(isLocationHasCGI(route)) {
                executeCGI();
            } else {
                // file does not have CGI
                // should return the requested file 200 OK
                std::string filename = "/" + index_files[i];
                fileHasNoCGI(route, filename);
            }
            break;
        } else {
            std::cout << "file not found" << std::endl;
        }
    }
}

void HTTPRequest::pathIsDirectory(std::map<std::string, Route> &routes, Route &route, const std::string &_path) {
    (void) routes;
    DIR *dir;

    if((dir = opendir(("/" + route.getRootDir() + _path).c_str())) != NULL) {
        const std::vector<std::string> index_files = route.getIndexFiles();
        // if must be redirected
        if(!route.getRedirect().empty()) {
            std::cout << "redirection ---> " << route.getRedirect() << std::endl;
            setPath(route.getRedirect());
            handleGet();
            return;
        }
        if(index_files.size() == 0)
            directoryHasNoIndexFiles(route);
        else
            directoryHasIndexFiles(route, index_files);
        closedir(dir);
    } else {
        setStatusCode(404);
        setStatusMessage("Not Found");
    }
}

void HTTPRequest::pathIsFile(std::map<std::string, Route> &routes, Route &route) {
    (void) routes;
    std::cout << "path is file: " << route.getRootDir() << std::endl;
    if(isLocationHasCGI(route)) {
        executeCGI();
    } else {
        // file does not have CGI
        // should return the requested file 200 OK
        std::string filename = "";
        fileHasNoCGI(route, filename);
    }
}

void HTTPRequest::fileHasNoCGI(Route &route, std::string &file_name) {
    (void) route;
    std::fstream file(("/" + route.getRootDir() + getPath() + file_name).c_str());
    if(!file.is_open()) {
        setFileContent("");
        return;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    setFileContent(ss.str());
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
    if(isDirectory(getPath(), route.getRootDir()))
        pathIsDirectory(routes, route, getPath());
    else
        pathIsFile(routes, route);
}

void HTTPRequest::handlePOST() {
    std::cout << "POST method" << std::endl;
}

void HTTPRequest::handleDELETE() {
    std::cout << "DELETE method" << std::endl;
}
