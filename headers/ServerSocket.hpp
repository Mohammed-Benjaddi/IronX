#pragma once
#include <string>
#include <sys/socket.h> //* socket() bind() listen() accept()
#include <netinet/in.h> //* sockaddr_in, IPPROTO_TCP
#include <arpa/inet.h>  //* inet_pton()
#include <unistd.h> //* close()
#include <fcntl.h> //* fcntl() F_SETFL O_NONBLOCK
#include <cstring> //* memset()
#include <iostream> //* cerr cout
#include <sstream> //* sstream
#include <stdio.h>
#include <netdb.h>

typedef unsigned char uint8_t; 

/**
 * @brief Represents one lsitening server socket host:port pair
 * Create a Socket -> Set it to non blocking -> bind to host:port 
 * - starts listening
 */
class ServerSocket {
    private:
        int         _fd;
        std::string _host;
        uint16_t    _port;
        int         _clientId;
    public:
        ServerSocket(const std::string&, uint16_t, int);
        ~ServerSocket();

        std::string getIP() const;
        int     getClusterId() const;
        int     getFd() const;
        void    setFd(int);
        void    create_sock();
        void    non_block();
        void    bind_socket();
        void    init_listen();
        class SocketCreationFailure : public std::exception {
            public:
                const char * what() const throw();
        };
};