#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(const std::string &raw_request, WebServerConfig *_config, int _clientId) : IHTTPMessage(), config(_config), clientId(_clientId) {
    parse(*this, raw_request);
    checkAllowedMethods(*this);
    handleRequest();
}

HTTPRequest::~HTTPRequest() {}

void HTTPRequest::setMethod(const std::string &method) {
    this->method = method;
}

void HTTPRequest::setHeaders(std::string line) {
    size_t pos = line.find(':');
    if (!isBodyFound() && pos == std::string::npos) {
        // if(!isBodyFound()) {
        setBodyFound(true);
        setBody("");
            return;
        // }
    } else if (!isBodyFound() && pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        setHeader(key, value);
    } else {
        line += "\r\n";
        setBody(getBody() + line);
        // std::cout << "line ----> " << line << std::endl;    
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

void HTTPRequest::setBodyFound(bool b) {
    this->bodyFound = b;
}

void HTTPRequest::setFormFile(std::vector<FormFile> &formFiles) {
    this->formFiles = formFiles;
}

WebServerConfig *HTTPRequest::getConfig() const {
    return config;
}

std::string HTTPRequest::getMethod() const {
    return method;
}

std::string HTTPRequest::getPath() const {
    return path;
}


bool HTTPRequest::isBodyFound() const {
    return bodyFound;
}

std::string HTTPRequest::getHTTPVersion() const {
    return httpVersion;
}

std::string HTTPRequest::getHeader(const std::string &key) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end())
        return it->second;
    return "";
}

// std::string findHeader(HTTPRequest &request, std::string key) {
//   std::map<std::string, std::string> headers = request.getHeaders();
//   std::map<std::string, std::string>::const_iterator it = headers.find(key);
//   if (it != headers.end())
//     return it ->second;
//   return "";
// }

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

std::string HTTPRequest::getRootDir() const {
    return rootDir;
}

void HTTPRequest::setRootDir(std::string rootDir) {
    this->rootDir = rootDir;
}

std::string HTTPRequest::getBoundary() const {

    std::string content_type = getHeader("Content-Type");
    int index = content_type.rfind("WebKitFormBoundary");
    // std::cout << "index ----> " << index << std::endl;
    std::string boundary = "";
    if(index != -1)
        boundary = content_type.substr(index);
    return boundary;
}

std::vector<FormFile> &HTTPRequest::getFormFiles() {
    return formFiles;
}
// Methods

std::vector<uint8_t> HTTPRequest::to_bytes() const {
    std::vector<uint8_t> bytes;
    return bytes;
}

void HTTPRequest::handleRequest() {
    std::cout << "handle request ====> " << getMethod() << std::endl;
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
    std::map<std::string, Route> routes = config->getClusters()[clientId].getRoutes();

    std::map<std::string, Route>::const_iterator it_route = routes.find(getPath());
    std::string pathToSearch;
    Route route;
    std::cout << "GET method" << std::endl;
    if(it_route == routes.end()) {
        // std::cout << "route not found: " <<  << std::endl;
        // route.setRootDir("home/simo/cursus/webserv/www/html");
        route.setRootDir(getRootDir());
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

void HTTPRequest::handleDELETE() {
    std::cout << "DELETE method" << std::endl;
    std::map<std::string, Route> routes = config->getClusters()[clientId].getRoutes();

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


void HTTPRequest::handlePOST() {
    std::cout << "POST method" << std::endl;
    // std::cout << getHeader("Content-Type") << std::endl;
    /*
        NOTES:
            * the request body always starts after \r\n\r\n
            * so, I think that I should build a struct (webKitFormBoundary) that contains the filename
            ,the content type and the content of the file, and then create a vector
            that stores the n webKitFormBoundary        
            check if location does not support upload
    */
    // std::vector<FormFile> formFiles = parseMultipartFormData(getBody(), getBoundary());
    std::vector<FormFile> formFiles = parseMultipartFormData(getBody(), getBoundary());
    setFormFile(formFiles);
    // std::cout << "form size ---> " << formFiles.size() << std::endl;
    uploadFiles(*this);
    // for (size_t i = 0; i < formFiles.size(); ++i) {
    //     std::cout << "File " << i + 1 << ":\n";
    //     std::cout << "  Name: " << formFiles[i].name << "\n";
    //     std::cout << "  Filename: " << formFiles[i].filename << "\n";
    //     std::cout << "  Content-Type: " << formFiles[i].contentType << "\n";
    //     std::cout << "  Data size: " << formFiles[i].data.size() << " bytes\n";
    //     // std::cout << "  Data preview: " << formFiles[i].data.substr(0, 50) << "\n";
    //     std::cout << "Data preview: ";
    //     for(size_t j = 0; j < formFiles[i].data.size(); j++) {
    //         std::cout << formFiles[i].data[j];
    //     }
    //     std::cout << std::endl;
    // }

}
