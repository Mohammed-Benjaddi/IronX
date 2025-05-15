#pragma once

#include <vector>
#include <string>
#include <stdint.h>

class Listener {
    private:
        std::string host;
        uint16_t    port;
        int         socket_fd;
    public:
        Listener(int, const std::string&, uint16_t port);

        int getSocketFd() const;
        const std::string& getHost() const;
        uint16_t getPort() const;

        void setSocketFd(int fd);
        void setHost(const std::string& h);
        void setPort(uint16_t p);
};


