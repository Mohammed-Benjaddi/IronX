#pragma once

#include <string>
#include "HTTPRequest.hpp"

bool isfileExist(const char* path);
bool isDirectory(const std::string path, std::string rootDir);
bool isLocationHasCGI(Route &route);
void copyToRoute(Route &route, std::map<std::string, Route>::const_iterator &it);