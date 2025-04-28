#include "../../headers/NetworkActions.hpp"

//! LISTENER CLASS !//

Listener::Listener(int fd, const std::string& h, uint16_t p) 
    : socket_fd(fd), host(h), port(p) {}

int Listener::getSocketFd() const {
    return socket_fd;
}

const std::string& Listener::getHost() const {
    return host;
}

uint16_t Listener::getPort() const {
    return port;
}

// Setters implementation
void Listener::setSocketFd(int fd) {
    socket_fd = fd;
}

void Listener::setHost(const std::string& h) {
    host = h;
}

void Listener::setPort(uint16_t p) {
    port = p;
}

