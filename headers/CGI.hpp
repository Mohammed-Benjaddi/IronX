#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <HTTPRequest.hpp>

class HTTPRequest;
class Route;

class CGI {
    private:
    std::string script_path;
    std::string request_method;
    std::string query_string;
    std::string request_body;
    std::string extension;
    std::string script_output;
    std::map<std::string, std::string> env_vars;
    HTTPRequest &request;
    Route &route;
    public:
    CGI(HTTPRequest &request, Route &route);
    void setupEnvironment();
    std::string intToString(int num);
    char** createEnvArray();
    void freeEnvArray(char** env_array);
    std::vector<std::string> getInterpreter(const std::string& script_path);
    std::string getScriptOutput() {
        return script_output;
    }
    void executeCGI();
};
