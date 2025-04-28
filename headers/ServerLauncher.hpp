#pragma once
#include "WebServerConfig.hpp"
#include "NetworkActions.hpp"

class   ServerLauncher {
    private:
        std::vector<Listener> listeners;
    public:
        ServerLauncher();
        ~ServerLauncher(); 
        void launch(const WebServerConfig &);
        // const std::vector<Listener>& getListeners() const;
};
