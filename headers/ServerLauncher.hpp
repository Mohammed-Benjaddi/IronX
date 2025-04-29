#pragma once
#include "WebServerConfig.hpp"
#include "NetworkActions.hpp"
#include "ServerSocket.hpp"

class   ServerLauncher {
    private:
        std::vector<ServerSocket> _sockets;
    public:
        ServerLauncher();
        ~ServerLauncher(); 
        void launch(const WebServerConfig &);
        // const std::vector<Listener>& getListeners() const;
};
