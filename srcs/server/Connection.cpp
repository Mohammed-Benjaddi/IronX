#include "../../headers/Connection.hpp"
#include "../../headers/Multiplexer.hpp"
#include "../../headers/HTTPRequest.hpp"
#include "../../headers/FileStreamer.hpp"

Connection::Connection()
    : _fd(-1), _readBuffer(""), _writeBuffer(""), _closed(false), _config(NULL), _streamer(NULL), _httpResponse(NULL) {};


// ! To be changed only serving static file now !!
Connection::Connection(int fd, int epoll_fd, WebServerConfig* config)
    : _fd(fd), _epoll_fd(epoll_fd), _readBuffer(""), _writeBuffer(""), _connectionHeader(""), _closed(false), _config(config), _streamer(NULL), _httpResponse(NULL) {};

std::string& Connection::getReadBuffer() {
    return this->_readBuffer;
}

std::string& Connection::getWriteBuffer() {
    return this->_writeBuffer;
}

void Connection::handleRead() {
    //! tmp buffer - CHUNK -

    char buffer[4096];
    while (true) {
        ssize_t bytes_read = recv(
            _fd, 
            buffer,
            sizeof(buffer), 
            0);
        
        if (bytes_read > 0) {
            _readBuffer.append(buffer, bytes_read);
        }
        else if (bytes_read == 0) {
            //? Client closed connection
            _closed = true;
            throw Multiplexer::ClientDisconnectedException();
        }
        else {
            //? No more data to read now
            break;
        }
    }

    //!  Merge Point Multiplexer <-> Request Branches   */

    _httpRequest = new HTTPRequest(_readBuffer, _config, 0);

    _httpResponse = new HTTPResponse(_httpRequest);

    //? build new epoll event modufying existing one

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
    //!!!!!!!!  SEGFAULT   
    std::cout << "Im writing\n";
    if (_httpResponse && _httpResponse->isComplete() && _writeBuffer.empty()) {
        std::string connType = _httpResponse->getConnectionHeader();
        delete _httpResponse;
        _httpResponse = NULL;       
        // if (connType == "close") {
        // 	close(_fd);
        // 	_closed = true;

		// 	return ;
        // } else {
		reset();
        re_armFd();
        // }
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




    // exit(1);
    // _streamer = new FileStreamer("/home/nab/Desktop/webserve-42/www/html/razzmatazz.mp3" , request.get_header("Connection"));
    

    //? *****************************/
    // char cwd[PATH_MAX];
    // getcwd(cwd, sizeof(cwd));
    // std::string fullPath = std::string(cwd);
    // fullPath += "/www/html";
    // fullPath += request.getPath();
    // if (request.getPath().empty())
    //     request.setPath("/index.html");
    // if (request.getRootDir().empty())
    //     request.setRootDir("/home/nab/Desktop/webserve-42/www/html");
    // fullPath = request.getRootDir() + request.getPath();
    //     fullPath += "index.html";
    
    
    // std::cout << request.getRootDir() << " " << request.getPath() << "\n";
    // _streamer = new FileStreamer(fullPath, request.getHeader("Connection"));
    // _httpResponse = new HTTPResponse(request.getHeader("Connection"), _streamer);
