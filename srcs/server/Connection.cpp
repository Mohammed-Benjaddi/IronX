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
    : _fd(fd),_epoll_fd(epoll_fd),_readBuffer(),_writeBuffer(),_connectionHeader(""),_headersPart(""),_closed(false),_config(config),_streamer(NULL),_httpResponse(NULL),_httpRequest(NULL),_headersParsed(false),_expectedBodyLength(0),_completedBuffer(false),_serverClusterId(serverClusterId),_hasCookie(false),_cookieHeader("")
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
    std::cout << _headersPart << "\033[95m" << std::endl;
    // sleep(10);
    // exit(0);
    start += std::string("Content-Length:").size();
    size_t end = _headersPart.find("\r\n", start);
    std::string value = _headersPart.substr(start, end - start);
    _expectedBodyLength = std::atol(value.c_str());
}

std::string generateSessionId() {
    std::string sessionId;
    static const char alphanum[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 16; ++i) {
        sessionId += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return sessionId;
}


void Connection::parseCookie() {
    const std::string bgCookieName = "bgColor";
    const std::string defaultBgUrl = "https://i.ibb.co/Gf7Nnfk2/team.png";

    size_t cookieStart = _headersPart.find("Cookie:");

    if (cookieStart != std::string::npos) {
        std::cout << "\033[33mClient has Cookie\033[0m\n";
        size_t lineEnd = _headersPart.find("\r\n", cookieStart);
        std::string cookieLine = _headersPart.substr(cookieStart + 7, lineEnd - (cookieStart + 7));


        size_t bgPos = cookieLine.find("bgColor=");
        if (bgPos != std::string::npos) {
            std::cout << "→ bgColor cookie found, no need to send new one.\n";
            return;
        }
        std::cout << "→ Cookie header found, but bgColor not set.\n";
    } else {
        std::cout << "\033[33mClient has NO Cookie\033[0m\n";
    }

    std::cout << "→ Creating new bgColor cookie\n";

    Cookie newCookie(bgCookieName, defaultBgUrl, 86400); 
    _cookieHeader = newCookie.getHeader();
    std::cout << "\033[33m" << _cookieHeader << "\033[33m\n";
}


void Connection::handleRead() {
    char buffer[4096];
    ssize_t bytes_read = recv(_fd, buffer, sizeof(buffer), 0);

    if (bytes_read > 0) {
        // Append new data to the read buffer
        _readBuffer.insert(_readBuffer.end(), buffer, buffer + bytes_read);

        // If headers haven't been parsed yet, look for the delimiter
        if (!_headersParsed) {
            const std::string delimiter = "\r\n\r\n";
            std::vector<char>::iterator it = std::search(
                _readBuffer.begin(),
                _readBuffer.end(),
                delimiter.begin(),
                delimiter.end()
            );

            if (it != _readBuffer.end()) {
                size_t pos = std::distance(_readBuffer.begin(), it);
                _headersPart = std::string(_readBuffer.begin(), _readBuffer.begin() + pos + delimiter.size());
                _headersParsed = true;
                parseCookie();
                parseContentLength();
            }
        }

        // If headers are parsed, check for complete body
        if (_headersParsed) {
            if ((_expectedBodyLength == 0) || (_readBuffer.size() >= (_headersPart.size() + _expectedBodyLength))) {
            //! Turning Vector into string for HTTPRequest
                std::string requestData(_readBuffer.begin(), _readBuffer.end());
                _httpRequest = new HTTPRequest(_readBuffer, _config, _serverClusterId);
                // _httpRequest = new HTTPRequest(requestData, _config, _serverClusterId);
                _httpResponse = new HTTPResponse(_httpRequest, _cookieHeader);
                re_armFd();
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
    std::cout << "Im writing\n";
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