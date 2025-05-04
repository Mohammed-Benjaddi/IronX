#include "parseRequest.hpp"


int checkAllowedMethods(HTTPRequest &request) {
  const std::map<std::string, Route>& routes = request.getConfig().getClusters()[request.getClientId()].getRoutes();
  std::map<std::string, Route>::const_iterator route_it = routes.find(request.getPath());
  const std::set<std::string> &allowedMethod = route_it->second.getAllowedMethods();

  if(allowedMethod.find(request.getMethod()) == allowedMethod.end()) {
      request.setStatusCode(405);
      request.setStatusMessage("Method Not Allowed");
      return -1;
  }
  return 1;
}

std::string findHeader(HTTPRequest &request, std::string key) {
  std::map<std::string, std::string> headers = request.getHeaders();
  std::map<std::string, std::string>::const_iterator it = headers.find(key);
  if (it != headers.end())
    return it ->second;
  return "";
}

bool URIHasUnallowedChar(std::string uri) {
  const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_-~/?#[]@!$$('*+,'=%.";
  const std::vector<char> allowedChars(chars.begin(), chars.end());
  
  for(size_t i = 0; i < uri.size(); i++) {
    if(std::find(allowedChars.begin(), allowedChars.end(), uri[i]) == allowedChars.end()) {
        std::cout << "unallowed char ---> " << uri[i] << std::endl;
        return true;
    }
  }
  return false;
}


int parse(HTTPRequest &request, std::fstream &file) {
  std::string line;
  std::stringstream ss;

  ss << file.rdbuf();
  // 413 error code if the request body larger than client max body in config file
  if(ss.str().length() > request.getConfig().getMaxBodySize()) {
      request.setStatusCode(413);
      request.setStatusMessage("Request Entity Too Large");
      return -1;
  }
  std::getline(ss, line);
  find_method_uri(request, line);
  while (std::getline(ss, line) && line != "\r")
  request.setHeaders(line);
  // 501 Not implemented
  if (!findHeader(request, "Transfer-Encoding").empty() && findHeader(request, "Transfer-Encoding") != "chunked") {
    request.setStatusCode(501);
    request.setStatusMessage("Not Implemented");
    return -1;
  }
  return 1;
}

bool checkRequestURI(HTTPRequest &request, std::string uri) {
  // 400 Bad request
  if(URIHasUnallowedChar(uri)) {
      request.setStatusCode(400);
      request.setStatusMessage("Bad request");
      return false;
  }
  // 414 Request-URI Too Long
  if(uri.length() > 2048) {
      request.setStatusCode(414);
      request.setStatusMessage("Request-URI Too Long");
      return false;
  }
  return true;
}

void find_method_uri(HTTPRequest &request, const std::string &line) {
  std::stringstream sstream(line);
  std::string method, uri, httpVersion;
  sstream >> method >> uri >> httpVersion;
  if(!checkRequestURI(request, uri)) {
      std::cout << "URI is not correct" << std::endl;
      return;
  }
  // Check if the uri contains a query string
  size_t queryPos = uri.find('?');
  if (queryPos != std::string::npos) {
      request.setQuery(uri.substr(queryPos + 1));
      request.setPath(uri.substr(0, queryPos));
  }
  request.setMethod(method);
  request.setPath(uri);
  request.setHTTPVersion(httpVersion);
}
