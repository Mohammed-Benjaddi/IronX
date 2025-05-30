#pragma once

#include "HTTPRequest.hpp"

class HTTPRequest;

typedef struct sFormFile FormFile;

int checkAllowedMethods(HTTPRequest &request);
int parse(HTTPRequest &request, const std::string &raw_request);
bool URIHasUnallowedChar(std::string uri);
bool checkRequestURI(HTTPRequest &request, std::string uri);
int find_method_uri(HTTPRequest &request, const std::string &line);
std::vector<FormFile> parseMultipartFormData(const std::string &body, const std::string &boundary);
std::string trim(const std::string& str);
std::string extractDirectory(const std::string& location);