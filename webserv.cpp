#include "./headers/WebServerConfig.hpp"
#include "mocker.hpp"

int main() {
    WebServerConfig config;

    mocker(config);
    
    printConfig(config);
    return (0);
}