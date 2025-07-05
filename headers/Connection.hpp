#pragma once

#include <iostream>
#include <filesystem>
#include <map>
#include <algorithm>
#include "../headers/WebServerConfig.hpp"
#include "../headers/FileStreamer.hpp"
#include "../headers/HTTPResponse.hpp"
#include "../headers/HTTPRequest.hpp"
#include "../headers/Cookie.hpp"

#define BLUE "\u001b[34m";

#define RED "\u001b[31m";

#define PINK "\033[95m";

#define LIME "\033[92m";

#define BROWN "\033[33m";

class Connection {
    public:
        Connection();
        Connection(int, int, WebServerConfig*, int serverClusterId);
        ~Connection(); 
        // std::string&    getReadBuffer();
        std::string&        getWriteBuffer();
        std::vector<char>&  getReadBuffer();
        // std::vector<char>& getWriteBuffer();
        void            parseContentLength();
        void            parseCookie();
        void            handleRead();
        void            handleWrite();
        bool            isClosed() const;
        void	        reset();
        void            re_armFd();
    private:
        int             _fd;
        int             _epoll_fd;
        // std::string     _readBuffer;
        std::vector<char> _readBuffer;
        std::string     _writeBuffer;
        // std::vector<char> _writeBuffer;
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
        bool            _hasCookie;
        std::string     _cookieHeader;
        
        // Connection& operator=(const Connection& other);
};