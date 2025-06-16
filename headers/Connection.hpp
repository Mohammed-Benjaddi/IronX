#pragma once

#include <iostream>
#include <filesystem>
#include "../headers/WebServerConfig.hpp"
#include "../headers/FileStreamer.hpp"
#include "../headers/HTTPResponse.hpp"
#include "../headers/HTTPRequest.hpp"

#define BLUE "\u001b[34m";

#define RED "\u001b[31m";

#define PINK "\033[95m";

#define LIME "\033[92m";

class Connection {
    public:
        Connection();
        Connection(int, int, WebServerConfig*, int serverClusterId);
        // ~Connection();
        // Connection& operator=(const Connection& other);
        std::string&    getReadBuffer();
        std::string&    getWriteBuffer();
        void            parseContentLength();
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
        std::string     _headersPart;
        bool            _closed;
        WebServerConfig *_config;
        FileStreamer    *_streamer;
        HTTPResponse    *_httpResponse;
        HTTPRequest     *_httpRequest;
        bool            _headersParsed;
        size_t          _expectedBodyLength;
        bool            _completedBuffer;
        int             _serverClusterId;
        
        // Connection& operator=(const Connection& other);
};