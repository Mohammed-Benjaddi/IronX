#include "../../headers/Multiplexer.hpp"

void	Multiplexer::poll_create() {
    /* Create an epoll instance */
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create failed");
        throw std::runtime_error("Failed to create epoll instance");
    }
}

void	Multiplexer::fds_register() {
    //* for each ServerSocket (listen)
    for (size_t i = 0; i < serverSockets.size(); ++i) {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = serverSockets.at(i).getFd();
        /* For each socket add it to epoll using controller api */
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSockets.at(i).getFd(), &ev) == -1) {
            std::cerr << "epoll_ctl failed" << std::endl;
            throw std::runtime_error("Failed to add server socket to epoll");
        }
    }
}

bool    Multiplexer::is_server_socket(int fd) {
    for (size_t i = 0; i < serverSockets.size(); ++i) {
        if (serverSockets.at(i).getFd() == fd)
            return true;
    }
    return false;
}

void	Multiplexer::run() {
    this->poll_create();
    this->fds_register();

    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    while (true) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1)
            throw std::runtime_error("epoll_wait failed");
        for (int i = 0; i < num_events; ++i) {
            int fd = events[i].data.fd;
            /* ( refer to epoll.txt ) */
            if (is_server_socket(fd)) {
                handle_new_connection(fd);
            } //else {
            //     handle_client_event(fd);
            // }
        }
    }
}

void    Multiplexer::handle_client_event(int fd) {
    (void)fd;
}

void    Multiplexer::handle_new_connection(int server_fd) {
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
        close(client_fd);
        throw std::runtime_error("Failed to accept connection from client");
    }

    //* Set client fd to non blocking
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        std::cerr << "epoll_ctl client_fd" << std::endl;
        throw std::runtime_error("Failed to add client fd to epoll");
    }
    activeConnections[client_fd] = Connection(client_fd);
    std::cout << "Accepted new client: fd = " << client_fd << std::endl;

}