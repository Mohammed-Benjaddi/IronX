#include "../../headers/Connection.hpp"
#include "../../headers/Multiplexer.hpp"

Connection::Connection()
    : _fd(-1), _readBuffer(""), _writeBuffer(""), _closed(false) {
    this->_fd = -1;
    this->_closed = true;
}

Connection::Connection(int fd)
    : _fd(fd), _readBuffer(""), _writeBuffer(""), _closed(false) {
    this->_fd = fd;
    this->_closed = false;
}

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
    std::cout << "Read buffer: " << _readBuffer << std::endl; 
}


void Connection::handleWrite() {
    std::string response = "Hey this is multiplexer speaking, im still in development\n";
    _writeBuffer.append(response);
    while (!_writeBuffer.empty()) {
        ssize_t bytes_sent = send(
            _fd, 
            _writeBuffer.data(), 
            _writeBuffer.size(), 
            0);

        if (bytes_sent > 0) {
            _writeBuffer.erase(0, bytes_sent);
        }
        else {
            // On non-blocking socket, -1 likely means "can't send more now"
            break;
        }
    }
    std::cout << "Write buffer: " << _writeBuffer << std::endl;
    //? If everything is sent, disable EPOLLOUT
    // if (_writeBuffer.empty()) {
    //     // Call epoll_ctl to modify events to only EPOLLIN now
    //     struct epoll_event ev;
    //     ev.events = EPOLLIN;  // only listen for read now
    //     ev.data.fd = _fd;
    //     epoll_ctl(epoll_fd, EPOLL_CTL_MOD, _fd, &ev);
    // }
}