#pragma once

#include "HTTPRequest.hpp"

class HTTPRequest;

int checkAllowedMethods(HTTPRequest &request);
// std::string findHeader(HTTPRequest &request, std::string key);
int parse(HTTPRequest &request, const std::string &raw_request);
bool URIHasUnallowedChar(std::string uri);
bool checkRequestURI(HTTPRequest &request, std::string uri);
void find_method_uri(HTTPRequest &request, const std::string &line);