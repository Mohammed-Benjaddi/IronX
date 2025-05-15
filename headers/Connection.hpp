#pragma once

#include <iostream>
#include "../headers/WebServerConfig.hpp"

class Connection {
    public:
        Connection();
        Connection(int, int, WebServerConfig*);
        // Connection& operator=(const Connection& other);
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
        WebServerConfig *_config;
};