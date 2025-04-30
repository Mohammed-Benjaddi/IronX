#include "../../headers/Connection.hpp"

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

