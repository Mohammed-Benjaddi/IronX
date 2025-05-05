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
        void    register_fd(int, uint32_t);
        int     wait_for_epoll(struct epoll_event*, int);
        void    dispatch_event(const struct epoll_event &);
        bool    is_server_socket(int);
        void    handle_new_connection(int);
            int     accept_new_client(int);
            void    make_fd_non_blocking(int);
            void    add_fd_to_epoll(int, uint32_t);
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