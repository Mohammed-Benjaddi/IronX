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
        bool    is_server_socket(int fd);
        void    handle_new_connection(int fd);
        void    handle_client_event(int fd);
};