#pragma once

#include <string>
#include "HTTPRequest.hpp"
#include "WebServerConfig.hpp"

class HTTPRequest;

bool isFileExist(const char* path);
bool isDirExist(std::string path, std::string rootDir);
bool isDirectory(const std::string path, std::string rootDir);
void DELETEDirectory(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route, const std::string &_path);
bool isLocationHasCGI(std::string filepath);
void copyToRoute(Route &route, std::map<std::string, Route>::const_iterator &it);
void fileHasNoCGI(HTTPRequest &request, Route &route, std::string &file_name);
void directoryHasIndexFiles(HTTPRequest &request, Route &route, std::vector<std::string> index_files);
void directoryHasNoIndexFiles(HTTPRequest &request, Route &route);
std::vector<std::string> getDirectoryListing(const std::string& path, bool show_hidden);
void pathIsDirectory(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route, const std::string &_path);
void pathIsFile(HTTPRequest &request, std::map<std::string, Route> &routes, Route &route);
void autoIndexOfDirectory(Route &route);
bool isDirectoryEmpty(std::string path);
void deleteRequestedFile(HTTPRequest &request, std::string path, std::string filename);
void uploadFiles(HTTPRequest &request);
