#pragma once

#include <iostream>

class Connection {
    public:
        Connection();
        Connection(int, int);
        std::string&    getReadBuffer();
        std::string&    getWriteBuffer();
        void            handleRead();
        void            handleWrite();
        bool            isClosed() const;
    private:
        int         _fd;
        int         _epoll_fd;
        std::string _readBuffer;
        std::string _writeBuffer;
        bool        _closed;
};