#include "utils.hpp"

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
