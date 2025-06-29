#include "CGI.hpp"

CGI::CGI(HTTPRequest &_request, Route &_route) : request(_request), route(_route)
{
//    //std::cout << "++++++ " << request.getRootDir() + "/" + request.getPath() << std::endl;
    script_path = request.getRootDir() + "/" + request.getPath();
    request_method = request.getMethod();
    query_string = request.getQuery();
    extension = script_path.substr(script_path.rfind("."));
    request_body = "body must be here";
    script_output = "";
    setupEnvironment();
//    //std::cout << "query ---> " << query_string << std::endl;
}

void CGI::setupEnvironment()
{
    env_vars["REQUEST_METHOD"] = request_method;
    env_vars["QUERY_STRING"] = query_string;
    env_vars["CONTENT_LENGTH"] = intToString(request_body.length());
    env_vars["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    env_vars["SERVER_PROTOCOL"] = request.getHTTPVersion();
    env_vars["SCRIPT_NAME"] = script_path;
}

std::string CGI::intToString(int num)
{
    std::ostringstream ss;
    ss << num;
    return ss.str();
}

char **CGI::createEnvArray()
{
    char **env_array = new char *[env_vars.size() + 1];
    int i = 0;

    for (std::map<std::string, std::string>::iterator it = env_vars.begin();
         it != env_vars.end(); ++it)
    {
        std::string env_string = it->first + "=" + it->second;
        env_array[i] = new char[env_string.length() + 1];
        strcpy(env_array[i], env_string.c_str());
        i++;
    }
    env_array[i] = NULL;
    return env_array;
}

void CGI::freeEnvArray(char **env_array) {
    for (int i = 0; env_array[i] != NULL; i++) {
        delete[] env_array[i];
    }
    delete[] env_array;
}

std::vector<std::string> CGI::getInterpreter(const std::string &script_path)
{
    std::vector<std::string> interpreters;
    CGIConfig config = route.getCGIConfig();
    // std::vector<std::string> extensions = config.getExtensions();
    // std::vector<std::string>::iterator it = std::find(extensions.begin(), extensions.end(), extension);
    std::string interpreter = config.getInterpreter();
    interpreters.push_back(interpreter);
    // interpreters.push_back("/usr/bin/env");
    if (script_path.find(".py") != std::string::npos)
        interpreters.push_back("python3");
    else if (script_path.find(".php") != std::string::npos)
        interpreters.push_back("php");
    interpreters.push_back(script_path);
    return interpreters;
}

void CGI::executeCGI()
{
    int pipe_fd[2];
    int stdin_pipe[2];

    if (pipe(pipe_fd) == -1 || pipe(stdin_pipe) == -1)
    {
        request.setStatusCode(500);
        request.setStatusMessage("Internal Server");
        request.setPath(request.getErrorPages(request.getStatusCode()));
        //std::cout << "* CGI: creation failed" << std::endl;
    }

    //std::cout << "script_path ----> " << script_path << std::endl;

    pid_t pid = fork();

    if (pid == -1)
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        request.setStatusCode(500);
        request.setStatusMessage("Internal Server");
        request.setPath(request.getErrorPages(request.getStatusCode()));
        //std::cout << "* CGI: Fork() failed" << std::endl;
    }

    if (pid == 0)
    {
        close(pipe_fd[0]);
        close(stdin_pipe[1]);
        dup2(pipe_fd[1], 1);
        close(pipe_fd[1]);
        dup2(stdin_pipe[0], 0);
        close(stdin_pipe[0]);
        char **env_array = createEnvArray();
        std::vector<std::string> interpreter = getInterpreter(script_path);
        char *args[4];
        args[0] = const_cast<char *>(interpreter[0].c_str());
        args[1] = const_cast<char *>(interpreter[1].c_str());
        args[2] = const_cast<char *>(interpreter[2].c_str());
        args[3] = NULL;
        execve(interpreter[0].c_str(), args, env_array);
        std::cerr << "CGI execution failed" << std::endl;
        // exit(1);
    }
    else
    {
        close(pipe_fd[1]);
        close(stdin_pipe[0]);

        if (!request_body.empty()) {
            write(stdin_pipe[1], request_body.c_str(), request_body.length());
        }
        close(stdin_pipe[1]);

        std::string output;
        char buffer[4096];
        ssize_t bytes_read;

        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            output += buffer;
        }

        close(pipe_fd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
           //std::cout << "the script executed successfully" << std::endl;
            script_output = output;
            request.setStatusCode(200);
            request.setStatusMessage("Ok");
        }
        else {
           //std::cout << "---------> something went wrong" << std::endl;
           //std::cout << "Exit status: " << WEXITSTATUS(status) << std::endl;
           //std::cout << "Script output: [" << output << "]" << std::endl;
            request.setStatusCode(500);
            request.setStatusMessage("Internal Server");
            request.setPath(request.getErrorPages(request.getStatusCode()));
           //std::cout << "* CGI: script execution failed" << std::endl;
        }
    }
}
