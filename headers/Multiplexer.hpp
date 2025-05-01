#pragma once

#include <vector>
#include <map>
#include <sys/epoll.h>
#include "ServerSocket.hpp"
#include "Connection.hpp"

class   Multiplexer {
    private:
    //* Connected Clients (starting empty)
        std::map<int, Connection> activeConnections; 
    //* Listening Sockets
        std::vector<ServerSocket> serverSockets;

        int epoll_fd;
    public:
        Multiplexer(const std::vector<ServerSocket> &socks) 
            : serverSockets(socks), epoll_fd(-1) {}
        void    run();
        void    poll_create();
        void    fds_register();
        bool    is_server_socket(int);
        void    handle_new_connection(int);
        void    handle_client_event(int, uint32_t);
        void    close_connection(int);
        class ClientDisconnectedException : public std::exception {
            public:
                const char* what() const throw();
        };

        class RecvFailureException : public std::exception {
            public:
                const char* what() const throw();
        };

        class SendFailureException : public std::exception {
            public:
                const char* what() const throw();
        };
};