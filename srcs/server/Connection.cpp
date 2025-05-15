#include "../../headers/Connection.hpp"
#include "../../headers/Multiplexer.hpp"

Connection::Connection()
    : _fd(-1), _readBuffer(""), _writeBuffer(""), _closed(false) {}

Connection::Connection(int fd, int epoll_fd)
    : _fd(fd), _epoll_fd(epoll_fd), _readBuffer(""), _writeBuffer(""), _closed(false) {}

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
    
    //? build new epoll event modufying existing one
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLOUT;
    ev.data.fd = _fd;
    epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev);
    
}


void Connection::handleWrite() {
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 17\r\n\r\nHello from server!";
    _writeBuffer.append(response);
    while (!_writeBuffer.empty()) {
        ssize_t bytes_sent = send(
            _fd, 
            _writeBuffer.data(), 
            _writeBuffer.size(), 
            0);

        if (bytes_sent > 0) {
            _writeBuffer.erase(0, bytes_sent);
        } else {
            // On non-blocking socket, -1 likely means "can't send more now"
            return ;
        }
    }
    //? If we reach here, it means the write buffer is empty
        //? We can modify the epoll event to only listen for read events
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.fd = _fd;
    epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev);
}