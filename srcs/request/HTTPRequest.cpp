#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::vector<char> &raw_request, WebServerConfig *_config, int _clientId) : IHTTPMessage(), config(_config), clientId(_clientId), cgi(NULL) {
    std::cout << "-----------\n";
    std::cout << "size: " << raw_request.size() << "\n";
    std::cout << "-----------\n";    
    if (parse(*this, raw_request) == -1)
        return;        
    if (checkAllowedMethods(*this) == -1)
        return;
    handleRequest();
}

HTTPRequest::~HTTPRequest() {}

void HTTPRequest::setMethod(const std::string &method) {
    this->method = method;
}

void HTTPRequest::setHeaders(std::string line)
{
    size_t pos = line.find(':');
    if (pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        setHeader(key, value);
    }
}

void HTTPRequest::setPath(const std::string &path)
{
    this->path = path;
}

void HTTPRequest::setHTTPVersion(const std::string &version)
{
    this->httpVersion = version;
}

void HTTPRequest::setHeader(const std::string &key, const std::string &value)
{
    headers[key] = value;
}

void HTTPRequest::setBody(const std::vector<char> &body)
{
    this->body = body;
}

void HTTPRequest::setQuery(const std::string &query)
{
    this->query = query;
}

void HTTPRequest::setFileContent(const std::string &fileContent)
{
    this->fileContent = fileContent;
}

void HTTPRequest::setClientId(int _clientId)
{
    this->clientId = _clientId;
}

void HTTPRequest::setBodyFound(bool b)
{
    this->bodyFound = b;
}

void HTTPRequest::setLocation(const std::string &location)
{
    this->location = extractDirectory(location);
}

void HTTPRequest::setFormFile(std::vector<FormFile> &formFiles)
{
    this->formFiles = formFiles;
}

void HTTPRequest::setErrorPages(const std::map<int, std::string> &error_pages)
{
    this->error_pages = error_pages;
}


// ---------------------------------------------------

WebServerConfig *HTTPRequest::getConfig() const
{
    return config;
}

std::string HTTPRequest::getMethod() const
{
    return method;
}

std::string HTTPRequest::getPath() const
{
    return path;
}

bool HTTPRequest::isBodyFound() const
{
    return bodyFound;
}

std::string HTTPRequest::getHTTPVersion() const
{
    return httpVersion;
}

std::string HTTPRequest::getHeader(const std::string &key) const
{
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

std::vector<char> HTTPRequest::getBody() const
{
    return body;
}

std::string HTTPRequest::getLocation() const
{
    return location;
}

std::map<std::string, std::string> HTTPRequest::getHeaders() const
{
    return headers;
}

std::string HTTPRequest::getHeaderValue(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end())
    {
        return it->second;
    }
    return "";
}

std::string HTTPRequest::getQuery() const
{
    return query;
}

std::string HTTPRequest::getFileContent() const
{
    return fileContent;
}

int HTTPRequest::getClientId() const
{
    return clientId;
}

std::string HTTPRequest::getRootDir() const
{
    return rootDir;
}

void HTTPRequest::setRootDir(std::string rootDir)
{
    this->rootDir = rootDir;
}


// ? "----WebKitFormBoundary9cvF8eER4QLomYfB"
std::string HTTPRequest::getBoundary() const {
    const std::string& content_type = getHeader("Content-Type");
    std::string key = "boundary=";
    size_t pos = content_type.find(key);
    if (pos == std::string::npos)
        return "";
    pos += key.length();
    std::string boundary = content_type.substr(pos);
    if (!boundary.empty() && boundary[0] == '"') {
        boundary.erase(0, 1); // Remove opening quote
        size_t end_quote = boundary.find('"');
        if (end_quote != std::string::npos)
            boundary = boundary.substr(0, end_quote);
    } else {
        size_t end = boundary.find_first_of(" ;");
        if (end != std::string::npos)
            boundary = boundary.substr(0, end);
    }
    return boundary;
}


std::vector<FormFile> &HTTPRequest::getFormFiles()
{
    return formFiles;
}

std::string HTTPRequest::getErrorPages(int code) const
{
    if (error_pages.find(code) != error_pages.end())
        return getRootDir() + error_pages.at(code);
    return "***";
}

// Methods

std::vector<uint8_t> HTTPRequest::to_bytes() const
{
    std::vector<uint8_t> bytes;
    return bytes;
}

void HTTPRequest::setFileExtension(const std::string &path)
{
    size_t slashPos = path.find_last_of("/\\");
    std::string filename = (slashPos == std::string::npos) ? path : path.substr(slashPos + 1);

    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == filename.length() - 1)
    {
        fileExtension = "";
        return;
    }

    fileExtension = filename.substr(dotPos + 1);
}

std::string HTTPRequest::getFileExtension()
{
    return fileExtension;
}

int HTTPRequest::setRoutesInfo(std::map<std::string, Route> &routes, Route &route)
{
    routes = config->getClusters()[clientId].getRoutes();
    std::map<std::string, Route>::const_iterator it_route = routes.find(getLocation());
    if (it_route == routes.end() && getLocation() != "/favicon.ico")
    {
        setStatusCode(404);
        setStatusMessage("Not Found");
        setPath(getErrorPages(getStatusCode()));
        return -1;
    }
    else
    {
        if (copyToRoute(*this, route, it_route) == -1)
            return -1;
    }
    return 1;
}

void HTTPRequest::handleRequest()
{
    Route route;
    std::map<std::string, Route> routes;

    if (setRoutesInfo(routes, route) == -1)
        return;
    setFileExtension(getPath());
    if (getMethod() == "GET")
        handleGet(routes, route);
    else if (getMethod() == "POST")
        handlePOST(routes, route);
    else if (getMethod() == "DELETE")
        handleDELETE(routes, route);
}

void HTTPRequest::executeCGI(Route &route)
{
    if (getMethod() == "DELETE")
        deleteRequestedFile(*this, "/" + route.getRootDir() + getPath(), "");
    else {
        CGIConfig cgiConfig = route.getCGIConfig();
        std::string path = getPath();
        std::vector<std::string> extensions = cgiConfig.getExtensions();
        std::string path_ext = path.substr(path.rfind("."));
        std::vector<std::string>::iterator it = std::find(extensions.begin(), extensions.end(), path_ext);
        if (it == extensions.end())
        {
            setStatusCode(404);
            setStatusMessage("Not Found");
            setPath(getErrorPages(getStatusCode()));
        } else {
            cgi = new CGI(*this, route);
            cgi->executeCGI();
        }
    }
}

void HTTPRequest::handleGet(std::map<std::string, Route> &routes, Route &route)
{
    if (isDirectory(getPath(), route.getRootDir()))
        pathIsDirectory(*this, routes, route, getPath());
    else
        pathIsFile(*this, routes, route);
}

void HTTPRequest::handleDELETE(std::map<std::string, Route> &routes, Route &route)
{
    if (isDirExist(getPath(), route.getRootDir()))
        DELETEDirectory(*this, routes, route, getPath());
    else
        pathIsFile(*this, routes, route);
}

void HTTPRequest::RedirectionFound(Route &route) {
    std::string a = getPath(), b = getLocation();
    setPath(route.getRedirect());
    setLocation(getPath());
    setStatusCode(301);
    setStatusMessage("Moved Permanently");
    setMethod("GET");
    handleRequest();
}

void HTTPRequest::handlePOST(std::map<std::string, Route> &routes, Route &route)
{
    std::string contentType = getHeader("Content-Type");
    if (isDirExist(getPath(), route.getRootDir())) {
        if (contentType.rfind("application/x-www-form-urlencoded") != std::string::npos) {
            setStatusCode(400);
            setStatusMessage("Bad Request");
            setPath(getErrorPages(getStatusCode()));
            return;
        }
        // if(getBody().empty())
        //     exit(0);
        std::vector<FormFile> formFiles = parseMultipartFormData(getBody(), getBoundary());
        setFormFile(formFiles);

        if (getFormFiles().empty()) {
           std::cout << "no form files" << std::endl;
           return;
        }
        uploadFiles(*this);
    } else {
        pathIsFile(*this, routes, route);
    }
}

CGI* HTTPRequest::getCGI() const { return cgi; }