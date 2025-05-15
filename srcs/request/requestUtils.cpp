#include "requestUtils.hpp"
// #include "HTTPRequest.hpp"

bool isFileExist(const char* path) {
    struct stat buffer;
    if (stat(path, &buffer) != 0) {
        return false;
    }
    return S_ISREG(buffer.st_mode);
}

bool isDirExist(std::string path, std::string rootDir) {
    struct stat buffer;
    std::string location = "/" + rootDir + path;
    if(location[location.length() - 1] == '/')
        location = location.substr(0, location.length() - 1);
    std::cout << "path to search ----> " << location << std::endl;
    if (stat(location.c_str(), &buffer) != 0) {
        std::cout << "it is not a directory" << std::endl;
        return false;
    }
    std::cout << "it is a directory" << std::endl;
    return S_ISDIR(buffer.st_mode);
}

bool isDirectory(const std::string path, std::string rootDir) {
    DIR *dir;
    if(path[path.size() - 1] == '/')
        return true;
    std::cout << "check path ---> " << ("/" + rootDir + path + "/") << std::endl;
    if ((dir = opendir(("/" + rootDir + path + "/").c_str())) != NULL) {
        std::cout << "I found a folder: " << "/" + rootDir + path + "/" << std::endl;
        closedir(dir);
        return true;
    }
    return false;
}

bool isLocationHasCGI(Route &route) {
    (void) route;
    /*
        I should check the extention of the file and return true if it has CGI
        but if the file has CGI and the method is DELETE I should return false
        so the 
    */
    return false;
}

void copyToRoute(Route &route, std::map<std::string, Route>::const_iterator &it) {
    route.setRootDir(it->second.getRootDir());
    route.setIndexFiles(it->second.getIndexFiles());
    route.setAllowedMethods(it->second.getAllowedMethods());
    route.setAutoindex(it->second.isAutoindex());
    route.setRedirect(it->second.getRedirect());
    route.setUploadDir(it->second.getUploadDir());
    route.setCGIConfig(it->second.getCGIConfig());
}

void GETReadFileContent(HTTPRequest &request, std::string path) {
    std::ifstream file(path.c_str());
    if(!file.is_open()) {
        std::cout << "* could not open" << std::endl;
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setFileContent("");
        return;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    request.setFileContent(ss.str());
} 

void deleteRequestedFile(HTTPRequest &request, std::string path, std::string filename) {
    // check permision of the file
    std::ofstream file((path + filename).c_str());
    if(!file.is_open()) {
        std::cout << "* could not open" << std::endl;
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setFileContent("");
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
    std::string filePath = ("/" + route.getRootDir() + request.getPath() + file_name);
    if(request.getMethod() == "GET") {
        GETReadFileContent(request, filePath);
        std::cout << "file content: " << request.getFileContent() << std::endl;
    }
    else if(request.getMethod() == "DELETE") {
        std::cout << "a file must be deleted" << std::endl;
        deleteRequestedFile(request, "/" + route.getRootDir() + request.getPath(), file_name);
    }
}

void directoryHasIndexFiles(HTTPRequest &request, Route &route, std::vector<std::string> index_files) {
    std::cout << "index files: " << std::endl;
    for(size_t i = 0; i < index_files.size(); i++) {
        std::cout << "index file #" << i << " --> " << route.getRootDir() + "/" + request.getPath() + "/" +  index_files[i] << std::endl;
        std::string path = "/" + route.getRootDir() + "/" + request.getPath() + "/" + index_files[i];
        if(isFileExist(path.c_str())) {
            std::cout << "I found the file" << std::endl;
            if(isLocationHasCGI(route)) {
                request.executeCGI(route);
            } else {
                // file does not have CGI
                // should return the requested file 200 OK
                std::string filename = "/" + index_files[i];
                fileHasNoCGI(request, route, filename);
            }
            break;
        } else {
            std::cout << "file not found" << std::endl;
        }
    }
}

void pathIsFile(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route) {
    (void) routes;
    std::cout << "path is file: " << route.getRootDir() << std::endl;
    std::string filePath = ("/" + route.getRootDir() + request.getPath());
    if(!isFileExist(filePath.c_str())) {
        std::cout << "* file not found" << std::endl;
        request.setStatusCode(404);
        request.setStatusMessage("Not Found");
        request.setFileContent("");
        return;
    }
    if(isLocationHasCGI(route))
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
    } else {
        if(!isDirectoryEmpty(location)) {
            std::cout << "directory is not empty" << std::endl;
            request.setStatusCode(409);
            request.setStatusMessage("Conflict");
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

    if((dir = opendir(("/" + route.getRootDir() + _path).c_str())) != NULL) {
        const std::vector<std::string> index_files = route.getIndexFiles();
        // if must be redirected
        if(!route.getRedirect().empty()) {
            std::cout << "redirection ---> " << route.getRedirect() << std::endl;
            request.setPath(route.getRedirect());
            // this must be fixed cause it's not gonna always be a GET request - simon
            request.handleRequest();
            return;
        }
        if(index_files.size() == 0)
            directoryHasNoIndexFiles(request, route);
        else
            directoryHasIndexFiles(request, route, index_files);
        closedir(dir);
    } else {
        request.setStatusCode(404);
        request.setStatusMessage("Not Found");
    }
}

void directoryHasNoIndexFiles(HTTPRequest &request, Route &route) {
    if(!route.isAutoindex()) {
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
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
            return;
        }
    }
    std::vector<std::string> entries = getDirectoryListing(path, false);
    std::cout << "route ===> " << route.getRootDir() << std::endl;
    for(size_t i = 0; i < entries.size(); i++) {
        std::cout << "---> " << entries[i] << std::endl;
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
