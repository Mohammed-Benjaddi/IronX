#include "../../headers/Connection.hpp"
#include "../../headers/Multiplexer.hpp"
#include "../../headers/HTTPRequest.hpp"
#include "../../headers/FileStreamer.hpp"

Connection::Connection()
    : _fd(-1),
      _epoll_fd(-1),
      _readBuffer(),
      _writeBuffer(),
      _connectionHeader(""),
      _headersPart(""),
      _closed(false),
      _config(NULL),
      _streamer(NULL),
      _httpResponse(NULL),
      _httpRequest(NULL),
      _headersParsed(false),
      _expectedBodyLength(0),
      _completedBuffer(false),
      _serverClusterId(0),
      _hasCookie(false),
      _cookieHeader("")
{}

Connection::Connection(int fd, int epoll_fd, WebServerConfig* config, int serverClusterId)
    : _fd(fd),_epoll_fd(epoll_fd),_readBuffer(),_writeBuffer(),_connectionHeader(""),
    _headersPart(""),_closed(false),_config(config),_streamer(NULL),_httpResponse(NULL),
    _httpRequest(NULL),_headersParsed(false),_expectedBodyLength(0),_completedBuffer(false),
    _serverClusterId(serverClusterId),_hasCookie(false),_cookieHeader("")
{}

// std::string& Connection::getReadBuffer() {
//     return this->_readBuffer;
// }

std::string& Connection::getWriteBuffer() {
    return this->_writeBuffer;
}

std::vector<char>& Connection::getReadBuffer() {
    return this->_readBuffer;
}

void Connection::parseContentLength() {
    if (_headersPart.find("POST") == std::string::npos)
        return;

    size_t start = _headersPart.find("Content-Length:");
    if (start == std::string::npos)
        return;
    start += std::string("Content-Length:").size();
    size_t end = _headersPart.find("\r\n", start);
    std::string value = _headersPart.substr(start, end - start);
    _expectedBodyLength = std::atol(value.c_str());
}

std::string replaceBgColorUrl(const std::string& setCookieHeader, const std::string& newUrl) {
    const std::string key = "bgColor=";
    size_t keyPos = setCookieHeader.find(key);
    if (keyPos == std::string::npos)
        return setCookieHeader; // bgColor not found, return unchanged

    size_t valueStart = keyPos + key.length();
    size_t valueEnd = setCookieHeader.find(";", valueStart);
    if (valueEnd == std::string::npos)
        valueEnd = setCookieHeader.length(); // no semicolon found, assume end of string

    // Reconstruct the string with the new URL
    std::string updated = setCookieHeader.substr(0, valueStart);
    updated += newUrl;
    updated += setCookieHeader.substr(valueEnd);

    return updated;
}

void Connection::parseCookie() {
    const std::string bgCookieName = "bgColor";
    const std::string _serverBgColorValue = "https://i.ibb.co/Gf7Nnfk2/team.png"; // define locally

    size_t cookieStart = _headersPart.find("Cookie:");
    std::string clientValue;

    if (cookieStart != std::string::npos) {
        size_t lineEnd = _headersPart.find("\r\n", cookieStart);
        std::string cookieLine = _headersPart.substr(cookieStart + 7, lineEnd - (cookieStart + 7));

        size_t bgPos = cookieLine.find("bgColor=");
        if (bgPos != std::string::npos) {
            size_t valueStart = bgPos + 8;
            size_t valueEnd = cookieLine.find(";", valueStart);
            clientValue = (valueEnd != std::string::npos)
                          ? cookieLine.substr(valueStart, valueEnd - valueStart)
                          : cookieLine.substr(valueStart);
        }
    }

    if (clientValue.empty()) {
        // Client has no cookie at all → set default
        Cookie newCookie(bgCookieName, _serverBgColorValue, 86400);
        _cookieHeader = newCookie.getHeader();
    } else if (clientValue != _serverBgColorValue) {
        // Client sent a different value → accept it, but update the expiration
        Cookie newCookie(bgCookieName, clientValue, 86400);
        _cookieHeader = newCookie.getHeader();
    } else {
        // Value matches → do not resend Set-Cookie
        _cookieHeader = "";
    }
}



void Connection::handleRead() {
    char buffer[4096];
    ssize_t bytes_read = recv(_fd, buffer, sizeof(buffer), 0);

    if (bytes_read > 0) {
        _readBuffer.insert(_readBuffer.end(), buffer, buffer + bytes_read);

        if (!_headersParsed) {
            const std::string delimiter = "\r\n\r\n";
            std::vector<char>::iterator it = std::search(
            _readBuffer.begin(),
            _readBuffer.end(),
            delimiter.begin(),
            delimiter.end()
        );

         size_t pos = std::distance(_readBuffer.begin(), it);
        _headersPart = std::string(_readBuffer.begin(), _readBuffer.begin() + pos + delimiter.size());
        _headersParsed = true;

        if (_headersPart.find("Cookie:") != std::string::npos) {
            _hasCookie = true;
            parseCookie();
        }
        parseContentLength();

        // If headers are parsed, check for complete body
        if (_headersParsed) {
            std::cout << "\n\033[1;31m******************************\033[0m\n";            
            std::cout << "\033[1;32m== HTTP REQUEST ==\n" << _headersPart << "\033[0m\n";
            if ((_expectedBodyLength == 0) || (_readBuffer.size() >= (_headersPart.size() + _expectedBodyLength))) {
                _httpRequest = new HTTPRequest(_readBuffer, _config, _serverClusterId);
                _httpResponse = new HTTPResponse(_httpRequest, _cookieHeader);
                printResponse(_httpResponse, _httpRequest);
                std::cout << "\033[1;31m******************************\033[0m\n\n";
                // exit(0);
                re_armFd();
                //
            } else {
                return;
            }
        }
    } else {
        _closed = true;
        throw Multiplexer::ClientDisconnectedException();
    }
    re_armFd();
}
}

void Connection::re_armFd() {
    struct epoll_event ev;
    ev.data.fd = _fd;

    if (!_writeBuffer.empty() || (_httpResponse && !_httpResponse->isComplete())) {
        ev.events = EPOLLOUT | EPOLLRDHUP;
    } else {
        ev.events = EPOLLIN | EPOLLRDHUP;
    }

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev) == -1) {
        perror("epoll_ctl: re_armFd");
        _closed = true;
    }
}

void Connection::handleWrite() {
    if (_httpResponse && _httpResponse->isComplete() && _writeBuffer.empty()) {
        std::string connType = _httpResponse->getConnectionHeader();
        delete _httpResponse;
        _httpResponse = NULL;     
		reset();
        re_armFd();
        return ;
    }

    if (_httpResponse && !_httpResponse->isComplete()) {
        std::string chunk = _httpResponse->getNextChunk();
        if (!chunk.empty())
            _writeBuffer.append(chunk);
    }

    if (!_writeBuffer.empty()) {
        ssize_t bytesSent = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);

        if (bytesSent > 0)
            _writeBuffer.erase(0, bytesSent);
        else if (bytesSent == -1) {
            _closed = true;
            throw Multiplexer::ClientDisconnectedException();
        }
    }

    if ((!_writeBuffer.empty() || (_httpResponse && !_httpResponse->isComplete())) && !_closed)
        re_armFd();
}

void	Connection::reset() {
	_writeBuffer.clear();
	_readBuffer.clear();
	delete _httpResponse;
	_httpResponse = NULL;
	delete _streamer;
	_streamer = NULL;
}

bool	Connection::isClosed()	const {
    return _closed;
}