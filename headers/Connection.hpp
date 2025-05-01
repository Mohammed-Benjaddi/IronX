#pragma once

#include <iostream>

class Connection {
    public:
        Connection();
        Connection(int client_fd);
        std::string&    getReadBuffer();
        std::string&    getWriteBuffer();
        void    handleRead();
        void    handleWrite();
        bool    isClosed() const;
    private:
        int         _fd;
        std::string _readBuffer;
        std::string _writeBuffer;
        bool        _closed;
};