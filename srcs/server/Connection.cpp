#include "../../headers/Connection.hpp"
#include "../../headers/Multiplexer.hpp"
#include "../../headers/HTTPRequest.hpp"
#include "../../headers/FileStreamer.hpp"

Connection::Connection()
    : _fd(-1), _readBuffer(""), _writeBuffer(""), _closed(false), _config(NULL), _streamer(NULL), _httpResponse(NULL), _headersParsed(false), _expectedBodyLength(0) {};


Connection::Connection(int fd, int epoll_fd, WebServerConfig* config)
    : _fd(fd), _epoll_fd(epoll_fd), _readBuffer(""), _writeBuffer(""), _connectionHeader(""), _headersPart(""), _closed(false), _config(config), _streamer(NULL), _httpResponse(NULL), _headersParsed(false), _expectedBodyLength(0) {};

std::string& Connection::getReadBuffer() {
    return this->_readBuffer;
}

std::string& Connection::getWriteBuffer() {
    return this->_writeBuffer;
}

 void Connection::handleRead() {
    char buffer[4096];
    ssize_t bytes_read = recv(_fd, buffer, sizeof(buffer), 0);

    if (bytes_read > 0) {
        _readBuffer.append(buffer, bytes_read);

        if (!_headersParsed) {
            size_t pos = _readBuffer.find("\r\n\r\n");
            if (pos != std::string::npos) {
                _headersPart = _readBuffer.substr(0, pos + 4);
                _headersParsed = true;
                parseContentLength();
            }
        }
        if (_headersParsed) {
            if ((_expectedBodyLength == 0) || (_readBuffer.size() >= (_headersPart.size() + _expectedBodyLength))) {
                _httpRequest = new HTTPRequest(_readBuffer, _config, 0);
                _httpResponse = new HTTPResponse(_httpRequest);
                re_armFd();
            } else
                return ;
        }
    } else {
        _closed = true;
        throw Multiplexer::ClientDisconnectedException();
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
