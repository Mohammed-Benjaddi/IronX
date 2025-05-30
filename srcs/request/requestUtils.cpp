#include "requestUtils.hpp"
// #include "HTTPRequest.hpp"


// ! edit this
bool isFileExist(const char* path) {
    // struct stat buffer;
    // printf("----> file to look for ---> %s\n", path);
    // if (stat(path, &buffer) != 0) {
    //     return false;
    // }
    // return S_ISREG(buffer.st_mode);
    std::ifstream file(path);
    if(file.is_open()) {
        file.close();
        return true;
    }
    return false;
}

bool isDirExist(std::string path, std::string rootDir) {
    struct stat buffer;
    std::string location = rootDir + path;
    std::cout << "path to search ----> " << location << std::endl;
    
    if(location[location.length() - 1] == '/')
        location = location.substr(0, location.length() - 1);
    if (stat(location.c_str(), &buffer) != 0) {
        std::cout << "it is not a directory" << std::endl;
        return false;
    }
    std::cout << "it is a file" << std::endl;
    return true;
}

bool isDirectory(const std::string path, std::string rootDir) {
    DIR *dir;
    // ! watch this
    // if(path[path.size() - 1] == '/')
    //     return true;
    std::cout << "check path ---> " << (rootDir + "/" + path) << std::endl;
    if ((dir = opendir((rootDir + "/" + path).c_str())) != NULL) {
        std::cout << "I found a folder: " << rootDir + "/" + path << std::endl;
        closedir(dir);
        return true;
    }
    return false;
}

bool isLocationHasCGI(std::string filepath) {
    const char* cgi_extensions[] = {".py", ".php", ".js", NULL};
    size_t dot_pos = filepath.rfind('.');
    if (dot_pos == std::string::npos)
        return false;
    std::string extension = filepath.substr(dot_pos);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    for (int i = 0; cgi_extensions[i] != NULL; ++i) {
        if (extension == cgi_extensions[i]) {
            return true;
        }
    }
    return false;
}

void copyToRoute(Route &route, std::map<std::string, Route>::const_iterator &it) {
    route.setRootDir(it->second.getRootDir());
    route.setIndexFiles(it->second.getIndexFiles());

    std::set<std::string>::iterator iter = it->second.getAllowedMethods().begin();
    while(iter != it->second.getAllowedMethods().end()) {
        std::cout << "method ----> " << *iter << std::endl;
        iter++;
    }

    route.setAllowedMethods(it->second.getAllowedMethods());
    route.setAutoindex(it->second.isAutoindex());
    route.setRedirect(it->second.getRedirect());
    route.setUploadDir(it->second.getUploadDir());
    route.setCGIConfig(it->second.getCGIConfig());
}

void GETReadFileContent(HTTPRequest &request, std::string path) {
    // ! edit this
    // std::ifstream file((path.substr(0, path.size() - 1)).c_str());
    std::ifstream file(path.c_str());

    if(!file.is_open()) {
        std::cout << "* could not open: " << path << std::endl;
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setFileContent("");
        request.setPath(request.getRootDir() + "/errors/403.html");
        return;
    }
    // ! remove it later;
    std::cout << "file opened successfully: " << path << std::endl;
    request.setPath(path);
    // exit(0);
    std::stringstream ss;
    ss << file.rdbuf();
    request.setFileContent(ss.str());
    file.close();
} 

void deleteRequestedFile(HTTPRequest &request, std::string path, std::string filename) {
    // check permision of the file
    std::ofstream file((path + filename).c_str());
    if(!file.is_open()) {
        std::cout << "* could not open" << std::endl;
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setFileContent("");
        request.setPath(request.getRootDir() + "/errors/403.html");
        return;
    }
    int result = remove((path).c_str());
    if(!result) {
        std::cout << "the file removed successfully" << std::endl;
        request.setStatusCode(204);
        request.setStatusMessage("No Content");
    }
    else
        std::cout << "something went wrong" << std::endl;
}

void fileHasNoCGI(HTTPRequest &request, Route &route, std::string &file_name) {
    std::string filePath = (route.getRootDir() + file_name);
    if(request.getMethod() == "GET") {
        GETReadFileContent(request, route.getRootDir() + "/" + request.getPath() + file_name);
        // std::cout << "file content: " << request.getFileContent() << std::endl;
    }
    else if(request.getMethod() == "DELETE") {
        std::cout << "a file must be deleted" << std::endl;
        deleteRequestedFile(request, "/" + route.getRootDir() + request.getPath(), file_name);
    }
}

void directoryHasIndexFiles(HTTPRequest &request, Route &route, std::vector<std::string> index_files) {
    std::cout << "index files: " << std::endl;
    for(size_t i = 0; i < index_files.size(); i++) {
        std::cout << "|" << request.getPath() << "|" << std::endl; 
        std::string path = route.getRootDir() + "/" + request.getPath() + "/" + index_files[i];

        // std::cout << "index file #" << i << " --> " << route.getRootDir() + request.getPath() +  index_files[i] << std::endl;
        std::cout << "index file #" << i << " --> " << path << std::endl;

        // std::cout << "route dir ----> " <<route.getRootDir()  << std::endl;
        // std::cout << "request path ----> " << request.getPath()  << std::endl;
        // std::cout << "index file ----> " << index_files[i] << std::endl;

        if(isFileExist(path.c_str())) {
            std::cout << "I found the file" << std::endl;
            if(isLocationHasCGI(path)) {
                request.executeCGI(route);
            } else {
                // file does not have CGI
                // should return the requested file 200 OK
                std::string filename = "/" + index_files[i];
                std::cout << "indexfile " << route.getRootDir() + "/" + request.getPath() + filename << std::endl;

                // request.setPath(index_files[i]);
                request.setFileExtension(filename);
                fileHasNoCGI(request, route, filename);
                return;
            }
            break;
        } else        
            std::cout << "file not found" << std::endl;
    }
    request.setStatusCode(404);
    request.setStatusMessage("Not Found");
    request.setPath(request.getRootDir() + "/errors/404.html");
}

void pathIsFile(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route) {
    (void) routes;
    // std::cout << "path is file: " << route.getRootDir() << std::endl;
    std::string filePath = (route.getRootDir() + "/" + request.getPath());
    std::cout << " ********** " << filePath << "***********" << std::endl;
    if(!isFileExist((filePath).c_str())) {
        std::cout << "* file not found" << std::endl;
        request.setStatusCode(404);
        request.setStatusMessage("Not Found");
        request.setFileContent("");
        request.setPath(request.getRootDir() + "/errors/404.html");
        return;
    }
    if(isLocationHasCGI(filePath))
        request.executeCGI(route);
    else {
        std::cout << "file has no CGI" << std::endl;
        std::string filename = "";
        fileHasNoCGI(request, route, filename);
    }
}

void DELETEDirectory(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route, const std::string &_path) {
    DIR *dir;
    (void) routes;
    std::string location = "/" + route.getRootDir() + _path;
    if((dir = opendir(location.c_str())) == NULL) {
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setPath(request.getRootDir() + "/errors/403.html");
    } else {
        if(!isDirectoryEmpty(location)) {
            std::cout << "directory is not empty" << std::endl;
            request.setStatusCode(409);
            request.setStatusMessage("Conflict");
            request.setPath(request.getRootDir() + "/errors/409.html");
        } else {
            int result = remove((location).c_str());
            if(!result)
                std::cout << "the folder removed successfully" << std::endl;
            else
                std::cout << "something went wrong" << std::endl;
        }
    }
}

void pathIsDirectory(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route, const std::string &_path) {
    (void) routes;
    DIR *dir;
    // std::cout << "_path: " << _path << std::endl;
    // exit(0);
    // if(_path == "/")
    //     _path = " ";
    std::cout << "simo ===> |" << route.getRootDir() << "|" << std::endl;
    // std::cout << "simo ===> " << (route.getRootDir() + (_path == "/" ? "" : _path)) << std::endl;
    if((dir = opendir((route.getRootDir() + "/" + (_path == "/" ? "" : _path)).c_str())) != NULL) {
        const std::vector<std::string> index_files = route.getIndexFiles();

        // if must be redirected
        if(!route.getRedirect().empty()) {
            std::cout << "redirection ---> " << route.getRedirect() << std::endl;
            request.setPath(route.getRedirect());
            // this must be fixed cause it's not gonna always be a GET request - simon
            request.handleRequest();
            return;
        }
        if(index_files.size() == 0) {
            std::cout << "==========================" << std::endl;
            directoryHasNoIndexFiles(request, route);
        }
        else {
            // exit(0);
            directoryHasIndexFiles(request, route, index_files);
        }
        closedir(dir);
    } else {
        request.setStatusCode(404);
        request.setStatusMessage("Not Found");
        request.setPath(request.getRootDir() + "/errors/404.html");
    }
}

void directoryHasNoIndexFiles(HTTPRequest &request, Route &route) {
    if(!route.isAutoindex()) {
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setPath(request.getRootDir() + "/errors/403.html");
    } else
        autoIndexOfDirectory(route);
}

std::vector<std::string> getDirectoryListing(const std::string& path, bool show_hidden) {
    std::vector<std::string> entries;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string filename(ent->d_name);
            if (filename == "." || filename == "..") {
                continue;
            }
            if (!show_hidden && !filename.empty() && filename[0] == '.') {
                continue;
            }
            bool is_dir = (ent->d_type == DT_DIR);
            if (is_dir) {
                filename += "/";
            }
            entries.push_back(filename);
        }
        closedir(dir);
        std::sort(entries.begin(), entries.end());
    }
    return entries;
}

void autoIndexOfDirectory(Route &route) {
    // search for index.html
    std::cout << "start listing files" << std::endl;
    std::string indexes[2] = {"index.html", "index.htm"};
    std::string path = route.getRootDir() + "/";
    for(size_t i = 0; i < 2; i++) {
        if(isFileExist((path + indexes[i]).c_str())) {
            std::cout << "file ----> " << path + indexes[i] << std::endl;
            // return;
        }
    }
    std::vector<std::string> entries = getDirectoryListing(path, false);
    std::cout << "route ===> " << route.getRootDir() << std::endl;
    for(size_t i = 0; i < entries.size(); i++) {
        std::cout << "+++++ ---> " << entries[i] << std::endl;
    }
}

bool isDirectoryEmpty(std::string path) {
    DIR* dir = opendir(path.c_str());
    if (!dir)
        return false;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (std::strcmp(entry->d_name, ".") != 0 && std::strcmp(entry->d_name, "..") != 0) {
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
    return true;
}


void uploadFiles(HTTPRequest &request) {
    (void) request;
    std::cout << "upload files here ----> " << std::endl;
    std::vector<FormFile> files = request.getFormFiles();
    // std::cout 

    if(files.empty()) {
        std::cout << "empty form" << std::endl;
        return;
    }

    std::cout << "content type: " << files[0].contentType << std::endl;
    std::cout << "filename: " << files[0].filename << std::endl;
    std::cout << "name: " << files[0].name << std::endl;
    for(size_t i = 0; i < 50; i++)
        std::cout << files[0].data[i];
    std::cout << std::endl;

    std::ofstream file(files[0].filename.c_str(), std::ios::binary);

    file.write(&files[0].data[0], files[0].data.size());
    
    std::cout << std::hex << (0xFF & files[0].data[0]) << " " << (0xFF & files[0].data[1]) << "\n";

    file.close();
}
