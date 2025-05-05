#include "requestUtils.hpp"
// #include "HTTPRequest.hpp"

bool isfileExist(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
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

void fileHasNoCGI(HTTPRequest &request, Route &route, std::string &file_name) {
    (void) route;
    (void) request;

    std::fstream file(("/" + route.getRootDir() + request.getPath() + file_name).c_str());
    if(!file.is_open()) {
        request.setStatusCode(404);
        request.setStatusMessage("Not Found");
        request.setFileContent("");
        return;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    request.setFileContent(ss.str());
}

void directoryHasIndexFiles(HTTPRequest &request, Route &route, std::vector<std::string> index_files) {
    std::cout << "index files: " << std::endl;
    for(size_t i = 0; i < index_files.size(); i++) {
        std::cout << "index file #" << i << " --> " << route.getRootDir() + "/" + request.getPath() + "/" +  index_files[i] << std::endl;
        std::string path = "/" + route.getRootDir() + "/" + request.getPath() + "/" + index_files[i];
        if(isfileExist(path.c_str())) {
            std::cout << "I found the file" << std::endl;
            if(isLocationHasCGI(route)) {
                request.executeCGI();
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
    if(isLocationHasCGI(route)) {
        request.executeCGI();
    } else {
        // file does not have CGI
        // should return the requested file 200 OK
        std::string filename = "";
        fileHasNoCGI(request, route, filename);
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
            request.handleGet();
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
        // 403 Forbidden must be implemented here if autoindex is off
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
    } else {
        autoIndexOfDirectory(route);
        // return autoindex of the directory
    }
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
        if(isfileExist((path + indexes[i]).c_str())) {
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