#include "./headers/WebServerConfig.hpp"
#include "mocker.hpp"
#include "./headers/Parser.hpp"

int main() {
    // WebServerConfig config;

    // mocker(config);
    
    // printConfig(config);
	Parser parse;
	parse.MainParser();
    return (0);
}