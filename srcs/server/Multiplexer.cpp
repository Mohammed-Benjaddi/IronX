#include "../../headers/Multiplexer.hpp"

void	Multiplexer::poll_create() {
    /* Create an epoll instance */
    epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        std::cerr << "epoll_create failed " <<  std::endl;
        throw std::runtime_error("Failed to create epoll instance");
    }
}

void	Multiplexer::fds_register() {
    //* for each ServerSocket (listen)
    for (size_t i = 0; i < serverSockets.size(); ++i) {
        add_fd_to_epoll(serverSockets.at(i).getFd(), EPOLLIN | EPOLLRDHUP);
    }
}

int    Multiplexer::wait_for_epoll(struct epoll_event *events, int max_events) {
    int num_events = epoll_wait(epoll_fd, events, max_events, -1);
    if (num_events == -1) {
        throw std::runtime_error("epoll_wait failed");
    }
    return num_events;
}

bool    Multiplexer::is_server_socket(int fd) {
    for (size_t i = 0; i < serverSockets.size(); ++i) {
        if (serverSockets.at(i).getFd() == fd)
            return true;
    }
    return false;
}

void    Multiplexer::dispatch_event(const struct epoll_event &event) {
    int fd = event.data.fd;
    uint32_t event_flags = event.events;

    if (is_server_socket(fd)) {
        handle_new_connection(fd);
    } else {
        handle_client_event(fd, event_flags);
        // exit(0);
       //std::cout << "Client event on fd: " << fd << std::endl;
    }
}

void	Multiplexer::run() {
    this->poll_create();
    this->fds_register();
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    while (true) {
		int num_events = wait_for_epoll(events, MAX_EVENTS);
		for (int i = 0; i < num_events; ++i) {
		    dispatch_event(events[i]);
		}
	}
}

int    Multiplexer::accept_new_client(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    //* capture clients net info into sockaddr_in
    int client_fd = accept (
        server_fd,
        (struct sockaddr*)&client_addr,
        &client_len
    );

    if (client_fd == -1) {
        std::cerr << "Accept Failed !" << std::endl;
        throw std::runtime_error("Failed to accept connection from client");
    }
    return client_fd;
}

void    Multiplexer::make_fd_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    //* Set client fd to non blocking
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        close(fd);
        throw std::runtime_error("Failed to set client socket to non-blocking (Action Closed Client Fd)");
    }
}

// void    Multiplexer::add_fd_to_epoll(int fd, uint32_t events) {
//     struct epoll_event ev;
//     ev.events = events;
//     ev.data.fd = fd;
//     if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
//         std::cerr << "epoll_ctl client_fd" << std::endl;
//         close(fd);
//         throw std::runtime_error("Failed to add client fd to epoll (Action; Closed Client Fd)");
//     }
// }

void Multiplexer::add_fd_to_epoll(int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
        std::cerr << "epoll_fd: " << epoll_fd << ", fd: " << fd << ", events: " << events << std::endl;
        close(fd);
        throw std::runtime_error("Failed to add client fd to epoll (Action; Closed Client Fd)");
    }
}

int     Multiplexer::getServerSocketByFd(int fd) {
    for (size_t i = 0; i < serverSockets.size(); ++i) {
        if (serverSockets[i].getFd() == fd) {
            return serverSockets[i].getClusterId();
        }
    }
    return -1;
}

void    Multiplexer::handle_new_connection(int server_fd) {
    int client_fd = accept_new_client(server_fd);
    make_fd_non_blocking(client_fd);
    add_fd_to_epoll(client_fd, EPOLLIN /*| EPOLLRDHUP | EPOLLOUT*/);

    activeConnections[client_fd] = Connection(client_fd, epoll_fd, &_config, getServerSocketByFd(server_fd));
}

void Multiplexer::handle_client_event(int fd, uint32_t event) {
	try {
        //! Disconnection Detection
	    if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
            throw Multiplexer::ClientDisconnectedException();
	    }

        Connection &conn = activeConnections.at(fd);

        if (event & EPOLLIN) {
            conn.handleRead();
        }
        
        if (event & EPOLLOUT) {
            conn.handleWrite();

        }
	} catch (const std::exception &e) {
        Multiplexer::close_connection(fd);
	}
}

void    Multiplexer::close_connection(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    activeConnections.erase(fd);
}

//!			Multiplexer Exceptions

const char* Multiplexer::ClientDisconnectedException::what() const throw() {
    return "Client disconnected from socket";
}

const char* Multiplexer::RecvFailureException::what() const throw() {
    return "recv() failed for client socket";
}

const char* Multiplexer::SendFailureException::what() const throw() {
    return "send() failed for client socket";
}