#include "../../headers/responseUtils.hpp"
#include "../../headers/HTTPResponse.hpp"

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
            res->setStatus(req->getStatusCode(), req->getStatusMessage());
            res->setHeader("Content-Type", "text/html");
            res->setHeader("Content-Length", toString(req->getCGI()->getScriptOutput().size()));
            res->setHeader("Connection", "close");
            std::string body("");
            if (req->getStatusCode() == 200)
                body.append("<html><body><h1>200 OK </h1><p>CGI Script Executed Succesefully.</p></body></html>");
            else
                body.append("<html><body><h1>500 Internal Server Error</h1><p>CGI script failed to execute.</p></body></html>");
            body.append(req->getCGI()->getScriptOutput());
            res->setBody(body);
        return true;
    }
    return false;
}

bool    fileExists(std::string fullPath) {
    std::ifstream file(fullPath.c_str());

    return file.good();
}

void buildResponse(HTTPRequest* req, HTTPResponse* res) {

    const std::string connection = "keep-alive";

    const std::string contentType = "text/html";
    int status = req->getStatusCode();
    std::string path = req->getPath();

    bool hasFile = fileExists(path);
    int fileSize = hasFile ? getFileSize(path) : 0;

    if (handleCGI(req, res) ) {
        if (req->getCGI() != NULL) {
            req->deleteCGI();
        }
        return;
    }

    if (status == 9999) {
        req->setStatusCode(200);
        res->setHeader("Content-Type", contentType);
        res->setHeader("Connection", connection);
        if (!fileExists(req->getPath()))
            res->buildAutoIndexResponse(req);
        else
            res->setStreamer(new FileStreamer(path, connection));
        return;
    }
    switch (status) {
        case 200:
        case 301:
        case 403:
        case 404:
        case 405:
        case 508:
        case 400:
        case 204:
        case 201:
            res->setStatus(status, req->getStatusMessage());
            res->setHeader("Content-Type", contentType);
            res->setHeader("Connection", connection);
            if (hasFile) {
                res->setHeader("Content-Length", toString(fileSize));
                res->setStreamer(new FileStreamer(path, connection));
            } else {
                std::string html = "<html><body><h1>" + toString(status) + " " + req->getStatusMessage() + "</h1></body></html>";
                res->setBody(html);
                res->setHeader("Content-Length", toString(html.size()));
            }
            return;

        default:
            break;
    }
    std::string errBody = "<html><body><h1>500 Internal Server Error</h1></body></html>";
    res->setStatus(500, "Internal Server Error");
    res->setHeader("Content-Type", contentType);
    res->setHeader("Content-Length", toString(errBody.size()));
    res->setHeader("Connection", "close");
    res->setBody(errBody);
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