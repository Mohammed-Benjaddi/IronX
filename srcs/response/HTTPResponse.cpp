#include "../../headers/HTTPResponse.hpp"


std::map<std::string, std::string> HTTPResponse::_mimeTypes = initMimeTypes();

// Default constructor for manual building
HTTPResponse::HTTPResponse()
    : _statusCode(200), _statusMessage("OK"), _connectionType("keep-alive"),
      _streamer(NULL), _bodyPos(0), _headersSent(false), _complete(false) {
    _headers["Connection"] = _connectionType;
    _headers["Content-Type"] = "text/plain"; // default
}

// Constructor using a request — fallback to OK or error response
HTTPResponse::HTTPResponse(HTTPRequest* request)
    : _statusCode(200), _statusMessage("OK"), _connectionType("keep-alive"),
      _streamer(NULL), _request(request), _bodyPos(0), _headersSent(false), _complete(false) {
    
    if (request->getMethod() == "DELETE") {
        // exit(0);
        std::cout << "Status Code : " << request->getStatusCode() << std::endl;
        setStandardHeaders(this, "text/plain", 0, "close", request->getStatusCode(), request->getStatusMessage());
    } else if (!request->getStatusCode())
        build_OK_Response(request, this);
      else
        buildResponse(request, this);
}

HTTPResponse::~HTTPResponse() {
    delete _streamer;
    if (!_tempFilePath.empty()) {
        remove(_tempFilePath.c_str());
        _tempFilePath.clear();
    }
}

// Set status line
void HTTPResponse::setStatus(int code, const std::string& message) {
    _statusCode = code;
    _statusMessage = message;
}


// Set body-based response
void HTTPResponse::setBody(const std::string& body) {
    _body = body;
    _headers["Content-Length"] = toString(body.size());
}

// Set file streamer response
void HTTPResponse::setStreamer(FileStreamer* streamer) {
    _streamer = streamer;
    _headers["Content-Type"] = getMimeType(streamer->getPath());
    _headers["Content-Length"] = toString(streamer->getFileSize());
}

// Set connection
void HTTPResponse::setConnection(const std::string& connection) {
    _connectionType = connection;
    _headers["Connection"] = connection;
}

size_t HTTPResponse::getFileSize(std::string path) const {
    struct stat st;
    if (stat(path.c_str(), &st) == -1) {
        return 0;
    }
    return static_cast<size_t>(st.st_size);
}

void HTTPResponse::setHeader(const std::string &key, const std::string &value) {
    _headers[key] = value;
}

// Write headers then serve chunk from either file or string
std::string HTTPResponse::getNextChunk() {
    if (!_headersSent) {
        _headersSent = true;

        std::ostringstream oss;
        oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
        for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
            oss << it->first << ": " << it->second << "\r\n";
        oss << "\r\n";
        return oss.str();
    }

    if (_streamer) {
        std::string chunk = _streamer->getNextChunk(8192);
        if (_streamer->isEOF()) {
            _complete = true;
            if (!_tempFilePath.empty()) {
                remove(_tempFilePath.c_str());
                _tempFilePath.clear();
            }
        }
        return chunk;
    } else {
        if (_bodyPos >= _body.size()) {
            _complete = true;
            return "";
        }
    }

    if (_bodyPos < _body.size()) {
        std::string chunk = _body.substr(_bodyPos, 8192);
        _bodyPos += chunk.size();
        if (_bodyPos >= _body.size()) {
            _complete = true;
            if (!_tempFilePath.empty()) {
                remove(_tempFilePath.c_str());
                _tempFilePath.clear();
            }
        }
        return chunk;
    }

    _complete = true;
    return "";
}


//!  BASE ROUTE new-site   */
/*
    location: ->>>>/new-site
    path: ->>>>/home/nab/Desktop/webserve-42/www/new-site/index.html
    root: ->>>>/home/nab/Desktop/webserve-42/www
    Base Case :
    in each each entry should be : location + path divided remove Root part 
        we stay with : /new-site + entry

    make a function that gets you the relative path from route default
*/
 


void HTTPResponse::buildAutoIndexResponse(HTTPRequest *request) {

    std::string location = request->getLocation();
    std::string curPath = request->getPath();
    std:: string rootPath = request->getRootDir();

    std::string relative = getRelativePath(curPath, rootPath);

    std::string directoryPath = curPath.substr(0, curPath.find_last_of("/\\"));
    std::vector<std::string> entries = getDirectoryListing(directoryPath, false);

    std::ostringstream html;
    html << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>WebServ: AutoIndex</title>\n<link rel=\"stylesheet\" href=\"/styles.css\">\n</head>\n";
    html << "<body class=\"body\">\n<div class=\"overlay\"></div>\n<div class=\"content\">\n";
    html << "<h2>📁 Index of " << curPath << "</h2>\n";
    html << "<table class=\"table\"><thead><tr><th class=\"th\">Name</th><th class=\"th\">Size</th></tr></thead><tbody>\n";

    std::string basePath = curPath;

    for (std::vector<std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        const std::string& entry = *it;
        const std::string entryPathToRoute = relative + "/" + entry;

        html << "<tr><td><a class=\"td a\" href=\"" <<  entryPathToRoute << "\">📂 " << entry << "</a></td>";
        html << "<td class=\"td\">" << getFileSize(entryPathToRoute) << " B</td></tr>\n";
    }

    html << "</tbody></table>\n<a class=\"gta-btn\" href=\"/\">⬅ Back to home</a>\n</div>\n</body>\n</html>\n";

    std::string outputPath = directoryPath + "/index.html";
    std::ofstream outputFile(outputPath.c_str());

    outputFile << html.str();
    outputFile.close();
    _streamer = new FileStreamer(outputPath, _connectionType);
    setStandardHeaders(this, "text/html", _streamer->getFileSize(), "keep-alive", 200, "OK");
    _tempFilePath = outputPath;
}

// Used for setting Connection: header
std::string HTTPResponse::getConnectionHeader() {
    return _headers.count("Connection") ? _headers["Connection"] : "close";
}

bool HTTPResponse::isComplete() const {
    if (_streamer)
        return _headersSent && _streamer->isEOF();
    return _headersSent && (_bodyPos >= _body.size());
}
