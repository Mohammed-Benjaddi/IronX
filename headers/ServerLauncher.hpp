#pragma once

#include "WebServerConfig.hpp"
#include "ServerSocket.hpp"
#include <fstream>
#include <vector>

class   ServerLauncher {
    private:
        std::vector<ServerSocket> _sockets;
    public:
        ServerLauncher();
        ~ServerLauncher();
        void buildHostnames(std::vector<std::string> &hostnames, const std::string &host, std::ofstream &conf);
        void launch(const WebServerConfig &);
        const std::vector<ServerSocket>&	getSockets();
};