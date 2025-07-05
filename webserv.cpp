#include "headers/WebServerConfig.hpp"
#include "headers/HTTPRequest.hpp"
#include "headers/ServerLauncher.hpp"
#include "headers/Multiplexer.hpp"
#include "mocker.hpp"
#include "headers/Parser.hpp"

#include <sstream>

int main(int ac, char **av) {
    if (ac == 2) {
        WebServerConfig config;        
        Parser parser;
        try {
            std::string path(av[1]);
            parser.MainParser(path ,config);
        } catch(const std::exception& e) {
            std::cerr << "===> " << e.what() << '\n';
            return 1;
        }

        try {
                ServerLauncher  launcher;
                launcher.launch(config);
                Multiplexer     mux(launcher.getSockets(), config);
                mux.run();
        } catch (const std::exception &e) {
                std::cerr << "Error Launching Server: " << e.what() << std::endl;
        }
    }
    return (0);
}