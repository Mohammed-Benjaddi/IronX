#pragma once

#include <map>
#include <string>
#include <sstream>
#include "FileStreamer.hpp"
#include "HTTPRequest.hpp"
#include "requestUtils.hpp"


class HTTPRequest;
class HTTPResponse;

std::map<std::string, std::string> initMimeTypes();

std::string toString(int value);

void build_OK_Response(HTTPRequest* req, HTTPResponse* res);

void buildResponse(HTTPRequest* req, HTTPResponse* res);

void setStandardHeaders(HTTPResponse*, const std::string&, size_t , const std::string&, int, std::string);

std::string getRelativePath(const std::string& path, const std::string& rootPath);

