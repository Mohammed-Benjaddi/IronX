#pragma once

#include <iostream>
#include <filesystem>
#include "../headers/WebServerConfig.hpp"
#include "../headers/FileStreamer.hpp"
#include "../headers/HTTPResponse.hpp"

class Connection {
    public:
        Connection();
        Connection(int, int, WebServerConfig*);
        // ~Connection();
        // Connection& operator=(const Connection& other);
        std::string&    getReadBuffer();
        std::string&    getWriteBuffer();
        HTTPResponse*   getResponse();
        void            handleRead();
        void            handleWrite();
        bool            isClosed() const;
        void	        reset();
        void            re_armFd();
    private:
        int             _fd;
        int             _epoll_fd;
        std::string     _readBuffer;
        std::string     _writeBuffer;
        std::string     _connectionHeader;
        bool            _closed;
        WebServerConfig *_config;
        FileStreamer    *_streamer;
        HTTPResponse    *_httpResponse;
        // Connection& operator=(const Connection& other);
};