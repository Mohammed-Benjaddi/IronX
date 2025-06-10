#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(const std::string &raw_request, WebServerConfig *_config, int _clientId) : IHTTPMessage(), config(_config), clientId(_clientId)
{
    // std::cout << "raw \n" << raw_request.substr(0, 20) << std::endl;
    if (parse(*this, raw_request) == -1)
        return;
    // if(getMethod() == "POST") {
    //     std::cout << "---> " << getBody().substr(0, 300) << std::endl;
    //     exit(0);
    // }
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
    if (!isBodyFound() && pos == std::string::npos)
    {
        // if(!isBodyFound()) {
        setBodyFound(true);
        setBody("");
        return;
        // }
    }
    else if (!isBodyFound() && pos != std::string::npos)
    {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        setHeader(key, value);
    }
    else
    {
        // if(!line.length()) {
        //     exit(0);
        // }
        // std::cout << "line =====> " << line.length() << std::endl;
        line += "\r\n";
        setBody(getBody() + line);
        // std::cout << "line ----> " << line << std::endl;
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

void HTTPRequest::setBody(const std::string &body)
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

std::string HTTPRequest::getBody() const
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

std::string HTTPRequest::getBoundary() const
{
    std::string content_type = getHeader("Content-Type");
    int index = content_type.rfind("WebKitFormBoundary");
    // std::cout << "index ----> " << index << std::endl;
    std::string boundary = "";
    if (index != -1)
        boundary = content_type.substr(index);
    return trim(boundary);
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

    // std::cout << "**** " << filename.substr(dotPos + 1) << std::endl;
    fileExtension = filename.substr(dotPos + 1);
}

std::string HTTPRequest::getFileExtension()
{
    return fileExtension;
}

int HTTPRequest::setRoutesInfo(std::map<std::string, Route> &routes, Route &route)
{
    routes = config->getClusters()[clientId].getRoutes();
    // std::cout << "location: " << (getLocation() == "/new-site" ? "yes" : "no") << std::endl;
    // std::cout << "* * * location: " << getLocation() << std::endl;
    std::map<std::string, Route>::const_iterator it_route = routes.find(getLocation());
    if (it_route == routes.end() && getLocation() != "/favicon.ico")
    {
        setStatusCode(404);
        setStatusMessage("Not Found");
        setPath(getErrorPages(getStatusCode()));
        // std::cout << "waaaa3 ===> " << getLocation() << std::endl;
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
    // std::cout << "full path ===> " << getRootDir() + getPath() << std::endl;
}

void HTTPRequest::executeCGI(Route &route)
{

    std::cout << "a CGI file found" << std::endl;
    if (getMethod() == "DELETE")
    {
        std::cout << "a file must be deleted" << std::endl;
        deleteRequestedFile(*this, "/" + route.getRootDir() + getPath(), "");
    }
    else
    {
        CGIConfig cgiConfig = route.getCGIConfig();
        std::string path = getPath();
        std::vector<std::string> extensions = cgiConfig.getExtensions();
        // if(!getQuery().empty()) {
        //     size_t q = path.find(getQuery());
        //     if(q != std::string::npos)
        //         path = path.substr(0, q - 1); 
        // }
        std::string path_ext = path.substr(path.rfind("."));
        std::vector<std::string>::iterator it = std::find(extensions.begin(), extensions.end(), path_ext);
        std::cout << "path ext: " << path_ext << std::endl;
        std::cout << "ext size ---> " << extensions.size() << std::endl;
        // exit(0);
        if (it == extensions.end())
        {
            std::cout << "-----> extensions not found" << std::endl;
            setStatusCode(404);
            setStatusMessage("Not Found");
            setPath(getErrorPages(getStatusCode()));
        }
        else
        {

            //! add POST implementation here
            std::cout << "root ---> " << getRootDir() << std::endl;
            cgi = new CGI(*this, route);
            (void)cgi;
            cgi->executeCGI();
            // exit(0);
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
    {
        std::cout << "path is directory" << std::endl;
        DELETEDirectory(*this, routes, route, getPath());
    }
    else
        pathIsFile(*this, routes, route);
}

void HTTPRequest::RedirectionFound(Route &route) {
    setPath(route.getRedirect());
    setLocation(getPath());
    setStatusCode(301);
    setStatusMessage("Moved Permanently");
    setMethod("GET");
    handleRequest();
}

void HTTPRequest::handlePOST(std::map<std::string, Route> &routes, Route &route)
{
    std::cout << "POST method" << std::endl;
    // exit(0);
    std::string contentType = getHeader("Content-Type");
    std::cout << getBody();

    if (isDirExist(getPath(), route.getRootDir())) {
        std::cout << "path is directory" << std::endl;
        if(contentType.rfind("application/x-www-form-urlencoded") != std::string::npos) {
            setStatusCode(400);
            setStatusMessage("Bad Request");
            setPath(getErrorPages(getStatusCode()));
            return;
        }
        std::vector<FormFile> formFiles = parseMultipartFormData(getBody(), getBoundary());
        setFormFile(formFiles);

        if (getFormFiles().empty()) {
            std::cout << "no form files" << std::endl;
        }
        uploadFiles(*this);
    } else {
        pathIsFile(*this, routes, route);
    }
    // exit(0);
    // std::vector<FormFile> formFiles = parseMultipartFormData(getBody(), getBoundary());
    
}
