#include "../../headers/responseUtils.hpp"

std::map<std::string, std::string> initMimeTypes() {
    std::map<std::string, std::string> m;
    m[".html"] = "text/html";
    m[".htm"]  = "text/html";
    m[".css"]  = "text/css";
    m[".js"]   = "application/javascript";
    m[".json"] = "application/json";
    m[".jpg"]  = "image/jpeg";
    m[".jpeg"] = "image/jpeg";
    m[".png"]  = "image/png";
    m[".gif"]  = "image/gif";
    m[".mp3"]  = "audio/mpeg";
    m[".mp4"]  = "video/mp4";
    m[".pdf"]  = "application/pdf";
    m[".txt"]  = "text/plain";
    return m;
}

size_t getFileSize(std::string path) {
    struct stat st;
    if (stat(path.c_str(), &st) == -1) {
        return 0;
    }
    return static_cast<size_t>(st.st_size);
}

std::string HTTPResponse::getMimeType(const std::string& path) {
    size_t dotPos = path.find_last_of(".");
    if (dotPos == std::string::npos)
        return "text/plain";

    std::string ext = path.substr(dotPos);
    if (_mimeTypes.count(ext))
        return _mimeTypes[ext];
    return "text/plain";
}

std::string toString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

bool    handleCGI(HTTPRequest *req, HTTPResponse *res) {
    if (req->getCGI()) {
        if (req->getCGI()) {
            res->setStatus(200, "OK");
            res->setHeader("Content-Type", "text/html");
            res->setHeader("Content-Length", toString(req->getCGI()->getScriptOutput().size()));
            res->setHeader("Connection", "close");
            res->setBody(req->getCGI()->getScriptOutput());
        } else {
            res->setStatus(500, "Internal Server Error");
            res->setHeader("Content-Type", "text/html");
            res->setHeader("Content-Length", "97");
            res->setHeader("Connection", "close");
            res->setBody("<html><body><h1>500 Internal Server Error</h1><p>CGI script failed to execute.</p></body></html>");
        }
        return true;
    }
    return false;
}

void buildResponse(HTTPRequest* req, HTTPResponse* res) {
    int status = req->getStatusCode();
    int size = getFileSize(req->getPath());

    if (handleCGI(req, res) ) {
        return;
    }

    std::string connection = "keep-alive";
    std::string contentType = "text/html";

    if (status == 9999) {
        res->setStatus(200, "OK");
        res->setHeader("Content-Type", contentType);
        res->setHeader("Content-Length", toString(size));
        res->setHeader("Connection", connection);
        res->buildAutoIndexResponse(req);
        res->setStreamer(new FileStreamer(req->getPath(), connection));
        return;
    }

    switch (status) {
        case 508:
            res->setStatus(status, req->getStatusMessage());
            res->setHeader("Content-Type", contentType);
            res->setHeader("Content-Length", toString(size));
            res->setHeader("Connection", connection);
            break;
        case 404:
        case 403:
        case 405:
        case 301:
        case 200:
            res->setStatus(status, req->getStatusMessage());
            res->setHeader("Content-Type", contentType);
            res->setHeader("Content-Length", toString(size));
            res->setHeader("Connection", connection);
            res->setStreamer(new FileStreamer(req->getPath(), connection));
            return;
    }

    res->setStatus(500, "Internal Server Error");
    res->setHeader("Content-Type", connection);
    res->setHeader("Content-Length", toString(size));
    res->setHeader("Connection", "close");
} 

void printResponse(HTTPResponse* res, HTTPRequest* req) {
    if (!res || !req) {
        std::cerr << "printResponse: NULL pointer!\n";
        return;
    }
    
    std::cout << "\033[1;33m== HTTP Response ==\033[0m\n";
    
    std::cout << "\033[1;33mHTTP/1.1 " << req->getStatusCode() << " " << req->getStatusMessage() << "\033[0m\n";
    std::cout << "\033[1;33mContent-Type: " << res->getMimeType(req->getPath()) << "\033[0m\n";
    std::cout << "\033[1;33mContent-Length: " << res->getHeaders().at("Content-Length") << "\033[0m\n";
    std::cout << "\033[1;33mConnection: " << res->getHeaders().at("Connection") << "\033[0m\n";
    if (res->hasCookie()) {
        std::cout << "\033[1;33mSet-Cookie: " << res->getHeaders().at("Set-Cookie") << "\033[0m\n";
    }
}

void setStandardHeaders(HTTPResponse* res, const std::string& contentType, 
    size_t contentLength, const std::string& connection, int statusCode, std::string statusMessage) {
    res->setHeader("Content-Type", contentType);
    res->setHeader("Content-Length", toString(contentLength));
    res->setHeader("Connection", connection);
    res->setStatus(statusCode, statusMessage);
}

std::string getRelativePath(const std::string& path, const std::string& rootPath) {
    std::string relative;
    if (path.find(rootPath) == 0) {
        relative = path.substr(rootPath.length()); 
    } else {
        relative = path;
    }

    relative = relative.substr(0, relative.size() - 11);

    return relative;
}