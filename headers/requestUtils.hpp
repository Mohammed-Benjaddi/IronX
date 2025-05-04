#pragma once

#include <string>
#include "HTTPRequest.hpp"
#include "WebServerConfig.hpp"

class HTTPRequest;

bool isfileExist(const char* path);
bool isDirectory(const std::string path, std::string rootDir);
bool isLocationHasCGI(Route &route);
void copyToRoute(Route &route, std::map<std::string, Route>::const_iterator &it);
void fileHasNoCGI(HTTPRequest &request, Route &route, std::string &file_name);
void directoryHasIndexFiles(HTTPRequest &request, Route &route, std::vector<std::string> index_files);
void directoryHasNoIndexFiles(HTTPRequest &request, Route &route);
std::vector<std::string> getDirectoryListing(const std::string& path, bool show_hidden);
void pathIsDirectory(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route, const std::string &_path);
void pathIsFile(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route);
void autoIndexOfDirectory(Route &route);