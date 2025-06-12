#include "./headers/WebServerConfig.hpp"
#include "mocker.hpp"
#include "./headers/Parser.hpp"






int main(int ac, char **av) {
    // WebServerConfig config;

    // mocker(config);
    
    // printConfig(config);
	if(ac == 2)
	{
		Parser parse;
		std::string path = av[1];
		parse.MainParser(path);

	}
	else
		std::cout << "usage: ./webserv [configuration file]" << std::endl;
    return (0);
}