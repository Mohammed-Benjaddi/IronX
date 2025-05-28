#include "../../headers/HTTPResponse.hpp"


HTTPResponse::HTTPResponse(HTTPRequest* request)
	: _connection("keep-alive"), _headersSent(false), _complete(false) {
    std::cout << "Response Started\n";
    std::cout << "Response Path: " << request->getRootDir() << request->getPath() << std::endl;
    _streamer = new FileStreamer(request->getRootDir() + request->getPath(), _connection);
    _mimeType = getMimeType(_streamer->getPath());
    this->prepareHeaders();
};

std::map<std::string, std::string> HTTPResponse::initMimeTypes() {
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
    // m[".ico"]  = "image/x-icon";
    return m;
}

std::map<std::string, std::string> HTTPResponse::_mimeTypes = HTTPResponse::initMimeTypes();

std::string HTTPResponse::getMimeType(const std::string& path) {
    size_t dotPos = path.find_last_of(".");
    if (dotPos == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(dotPos);
    if (_mimeTypes.count(ext))
        return _mimeTypes[ext];
    return "application/octet-stream";
}

void    HTTPResponse::prepareHeaders() {
    std::ostringstream oss;

    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Type: " << _mimeType << "\r\n";
    oss << "Content-Length: " << _streamer->getFileSize() << "\r\n";
    oss << "Connection: " << _streamer->getConnectionHeader() << "\r\n";
    // oss << "Content-Disposition: inline; filename=\"razzmatazz.mp3\"" << "\r\n";
    oss << "\r\n";
    _headers = oss.str();
}

std::string HTTPResponse::getNextChunk() {
    if (!_headersSent) {
        _headersSent = true;
        std::ostringstream headers;
        headers << "HTTP/1.1 200 OK\r\n";
        headers << "Content-Type: " << _mimeType << "\r\n";
        headers << "Content-Length: " << _streamer->getFileSize() << "\r\n";
        headers << "Connection: " << _streamer->getConnectionHeader() << "\r\n";
        headers << "\r\n";
        return headers.str();
    }

    std::string chunk = _streamer->getNextChunk(8192);
    if (_streamer->isEOF())
        _complete = true;
    return chunk;
}

std::string HTTPResponse::getConnectionHeader() {
    return _streamer->getConnectionHeader();
}

bool HTTPResponse::isComplete() const {
    return _headersSent && _streamer && _streamer->isEOF();
}