#include "requestUtils.hpp"

// ! edit this
bool isFileExist(const char* path) {
    struct stat buffer;
    if (stat(path, &buffer) != 0) {
        return false;
    }
    return true;
}

bool isDirExist(std::string path, std::string rootDir) {
    std::string location = rootDir + "/" + path;
    if (location[location.length() - 1] == '/')
        location = location.substr(0, location.length() - 1);
    struct stat buffer;
    if (stat(location.c_str(), &buffer) != 0)
        return false;
    if (S_ISDIR(buffer.st_mode))
        return true;
    else
        return false;
}

bool isDirectory(const std::string path, std::string rootDir) {
    DIR *dir;

    if ((dir = opendir((rootDir + "/" + path).c_str())) != NULL) {
        closedir(dir);
        return true;
    }
    return false;
}

bool isLocationHasCGI(std::string filepath) {
    const char* cgi_extensions[] = {".py", ".php", NULL};
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

int copyToRoute(HTTPRequest &request, Route &route, std::map<std::string, Route>::const_iterator &it) {
    route.setRootDir(it->second.getRootDir());
    route.setIndexFiles(it->second.getIndexFiles());
    if (it->second.getAllowedMethods().find(request.getMethod()) == it->second.getAllowedMethods().end()) {
        request.setStatusCode(405);
        request.setStatusMessage("Method Not Allowed");
        request.setPath(request.getErrorPages(request.getStatusCode()));;
        return -1;
    }
    route.setAllowedMethods(it->second.getAllowedMethods());
    route.setAutoindex(it->second.isAutoindex());
    route.setRedirect(it->second.getRedirect());
    route.setUploadDir(it->second.getUploadDir());
    route.setCGIConfig(it->second.getCGIConfig());
    return 1;
}

void GETReadFileContent(HTTPRequest &request, std::string path) {
    std::ifstream file(path.c_str());
    if(!file.is_open()) {
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setFileContent("");
        request.setPath(request.getErrorPages(request.getStatusCode()));;
        return;
    }
    request.setPath(path);
    std::stringstream ss;
    ss << file.rdbuf();
    request.setFileContent(ss.str());
    file.close();
} 

void deleteRequestedFile(HTTPRequest &request, std::string path, std::string filename) {
    std::ofstream file((path + filename).c_str());
    if (!file.is_open()) {
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setFileContent("");
        request.setPath(request.getErrorPages(request.getStatusCode()));
        return;
    }

    int result = remove((path).c_str());
    if (!result) {
        request.setStatusCode(204);
        request.setStatusMessage("No Content");
    }
}

void fileHasNoCGI(HTTPRequest &request, Route &route, std::string &file_name) {
    if(request.getMethod() == "POST") {
        request.setStatusCode(405);
        request.setStatusMessage("Method Not Allowed");
        request.setPath(request.getErrorPages(request.getStatusCode()));;
    }
    std::string filePath = (route.getRootDir() + file_name);
    if (request.getMethod() == "GET")
        GETReadFileContent(request, route.getRootDir() + "/" + request.getPath() + file_name);
    else if (request.getMethod() == "DELETE")
        deleteRequestedFile(request, "/" + route.getRootDir() + "/" + request.getPath(), file_name);
}

void directoryHasIndexFiles(HTTPRequest &request, Route &route, std::vector<std::string> index_files) {
    for(size_t i = 0; i < index_files.size(); i++) {
        std::string path = route.getRootDir() + "/" + request.getPath() + "/" + index_files[i];
        if(isFileExist(path.c_str())) {
            if(isLocationHasCGI(path)) {
                request.executeCGI(route);
            } else {
                std::string filename = "/" + index_files[i];
                request.setFileExtension(filename);
                fileHasNoCGI(request, route, filename);
                return;
            }
            break;
        }
    }
    request.setStatusCode(404);
    request.setStatusMessage("Not Found");
    request.setPath(request.getErrorPages(request.getStatusCode()));;
}

void pathIsFile(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route) {
    (void) routes;
    std::string filePath = (route.getRootDir() + "/" + request.getPath());
    if (!isFileExist((filePath).c_str())) {
        request.setStatusCode(404);

        request.setStatusMessage("Not Found");
        request.setFileContent("");
        // request.setPath(request.getRootDir() + "/errors/404.html");
        request.setPath(request.getErrorPages(request.getStatusCode()));
        return;
    }
    if(!route.getRedirect().empty()) {
        request.RedirectionFound(route);
        return;
    }
    if (isLocationHasCGI(filePath))
        request.executeCGI(route);
    else {
        std::string filename = "";
        fileHasNoCGI(request, route, filename);
    }
}

void DELETEDirectory(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route, const std::string &_path) {
    DIR *dir;
    (void) routes;
   
    std::string location = "/" + route.getRootDir() + "/" + _path;
    if ((dir = opendir(location.c_str())) == NULL) {
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        request.setPath(request.getErrorPages(request.getStatusCode()));
    } else {
        if (!isDirectoryEmpty(location)) {
            request.setStatusCode(409);
            request.setStatusMessage("Conflict");
            // request.setPath(request.getRootDir() + "/errors/409.html");
            request.setPath(request.getErrorPages(request.getStatusCode()));;
        } else {
            // ! * warning : more tests on this
            remove((location).c_str());
            request.setStatusCode(204);
            request.setStatusMessage("No Content");
            return ;
        }
    }
}

void pathIsDirectory(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route, const std::string &_path) {
    (void) routes;
    DIR *dir;
    if((dir = opendir((route.getRootDir() + "/" + (_path == "/" ? "" : _path)).c_str())) != NULL) {
        const std::vector<std::string> index_files = route.getIndexFiles();
        if(!route.getRedirect().empty()) {
            request.RedirectionFound(route);
            return;
        }
        if (index_files.size() == 0) {
            directoryHasNoIndexFiles(request, route);
        }
        else
            directoryHasIndexFiles(request, route, index_files);
        closedir(dir);
    } else {
        request.setStatusCode(404);
        request.setStatusMessage("Not Found");
        request.setPath(request.getErrorPages(request.getStatusCode()));
    }
}

void directoryHasNoIndexFiles(HTTPRequest &request, Route &route) {
    if(!route.isAutoindex()) {
        request.setStatusCode(403);
        request.setStatusMessage("Forbidden");
        // request.setPath(request.getRootDir() + "/errors/403.html");
        request.setPath(request.getErrorPages(request.getStatusCode()));;
    } else {
        // exit(0);
        
        autoIndexOfDirectory(route, request);
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

void autoIndexOfDirectory(Route &route, HTTPRequest &request) {
    std::string indexes[2] = {"index.html", "index.htm"};
    std::string path = route.getRootDir() + "/" + request.getPath() + "/";
    for(size_t i = 0; i < 2; i++) {
        if(isFileExist((path + indexes[i]).c_str())) {
            request.setPath(path + indexes[i]);
            return;
        }
    }
    std::vector<std::string> entries = getDirectoryListing(path, false);
    request.setPath(path + "index.html");
    request.setStatusCode(9999);
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
    std::cout << "upload a file" << std::endl;
    std::vector<FormFile> files = request.getFormFiles();
    if(files.empty()) {
        std::cout << "no files found" << std::endl;
    }

    char buffer[1024];
    std::string new_path, original_path;
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        original_path = std::string(buffer);
        new_path = original_path + "/www" + request.getLocation();
    }
    std::cout << "new path: " << new_path << std::endl;
        // path = std::string(buffer) + "/www";
    // int newDir = chdir((request.getRootDir() + "/" + request.getLocation()).c_str());
    int newDir = chdir(new_path.c_str());
    if (newDir < 0) {
        std::cout << "chdir failed: " << new_path << std::endl;
        return;
    }
    std::ofstream file(files[0].filename.c_str(), std::ios::binary);
    file.write(&files[0].data[0], files[0].data.size());
    file.close();
    std::cout << "filename: " << files[0].filename << std::endl;
    request.setStatusCode(201);
    request.setStatusMessage("Created");
    chdir(original_path.c_str());
    // ! * warning: the loop for debugging, it must be deleted later
    // while(1) {

    // }
}
