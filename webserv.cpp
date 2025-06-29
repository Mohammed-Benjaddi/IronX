#include "headers/WebServerConfig.hpp"
#include "headers/HTTPRequest.hpp"
#include "headers/ServerLauncher.hpp"
#include "headers/Multiplexer.hpp"
#include "mocker.hpp"
#include "./headers/Parser.hpp"

int main(int ac, char **av) {
    WebServerConfig config;
	
	if (ac == 2)
	{
		Parser parse;
		std::string path = av[1];
    	
		ServerLauncher  launcher;
		parse.MainParser(path, config);
		try {
				launcher.launch(config);
				Multiplexer     mux(launcher.getSockets(), config);
				mux.run();

		} catch (const std::exception &e) {
				std::cerr << "Error Launching Server: " << e.what() << std::endl;
		}
	}
	else
		std::cout << "usage: ./webserv [configuration file]" << std::endl;
}