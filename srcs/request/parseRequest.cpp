#include "parseRequest.hpp"


int checkAllowedMethods(HTTPRequest &request) {
  const std::map<std::string, Route>& routes = request.getConfig()->getClusters()[request.getClientId()].getRoutes();
  ////std::cout << "req ---> " << request.getRootDir() + request.getPath() << std::endl;
  std::map<std::string, Route>::const_iterator route_it = routes.find(request.getPath());
  if(route_it != routes.end()) {
    const std::set<std::string> &allowedMethod  = route_it->second.getAllowedMethods();

    std::set<std::string>::iterator it = allowedMethod.begin();

    while(it != allowedMethod.end()) {
     //std::cout << "===> " << *it->begin() << std::endl;
      it++;
    }

    if(allowedMethod.find(request.getMethod()) == allowedMethod.end()) {
        request.setStatusCode(405);
        request.setStatusMessage("Method Not Allowed");
        request.setPath(request.getErrorPages(request.getStatusCode()));;
        return -1;
    }
  } else {
    ////std::cout << "+++ location not found" << std::endl;
    // request.setStatusCode(404);
    // request.setStatusMessage("Not Found");
    // request.setPath(request.getRootDir() + "/errors/404.html");
    // return -1;
  }

  return 1;
}

// std::string findHeader(HTTPRequest &request, std::string key) {
//   std::map<std::string, std::string> headers = request.getHeaders();
//   std::map<std::string, std::string>::const_iterator it = headers.find(key);
//   if (it != headers.end())
//     return it ->second;
//   return "";
// }

bool URIHasUnallowedChar(std::string uri) {
  const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_-~/?#[]@!$$()'*+,'=%.&";
  const std::vector<char> allowedChars(chars.begin(), chars.end());
  
  for(size_t i = 0; i < uri.size(); i++) {
    if(std::find(allowedChars.begin(), allowedChars.end(), uri[i]) == allowedChars.end()) {
       std::cout << "unallowed char ---> " << uri[i] << std::endl;
        return true;
    }
  }
  return false;
}

int parse( HTTPRequest &request, std::vector<char> &req) {
  std::string line;
  const char *delimiter = "\r\n\r\n";
  std::vector<char>::iterator it = std::search(
    req.begin(), req.end(),
    delimiter, delimiter + 4
  );
  if (it == req.end())
    return -1;
  std::string headers(req.begin(), it);
  std::stringstream ss(headers);
  request.setErrorPages(request.getConfig()->getErrorPages());
  char buffer[BUFSIZ];
  if (getcwd(buffer, sizeof(buffer)) != NULL)
    request.setRootDir(std::string(buffer) + "/www");
  std::getline(ss, line);
  if (find_method_uri(request, line) == -1)
    return -1;
  
  while (std::getline(ss, line)) {
    request.setHeaders(line);
  }
  
  std::vector<char>::iterator bodyStart = it + 4;
  if (bodyStart < req.end()) {
    std::vector<char> body(bodyStart, req.end());
    request.setBody(body);
  } else {
    std::vector<char> emptyBody;
    request.setBody(emptyBody);
  }

  // std::cout << "body size: " << request.getBody().size() << std::endl;
  if(request.getBody().size() > request.getConfig()->getMaxBodySize()) {
    // std::cout << "waaa3" << std::endl;
    request.setStatusCode(413);
    request.setStatusMessage("Payload Too Large");
    return -1;
  }

  return 1;
}

bool checkRequestURI(HTTPRequest &request, std::string uri) {
  // 400 Bad request
  if (URIHasUnallowedChar(uri)) {
      request.setStatusCode(400);
      request.setStatusMessage("Bad request");
      request.setPath(request.getErrorPages(400));
      exit(99);
      return false;
  }
  // 414 Request-URI Too Long
  if(uri.length() > 2048) {
      request.setStatusCode(414);
      request.setStatusMessage("Request-URI Too Long");
      // request.setPath(request.getRootDir() + "/errors/414.html");
      request.setPath(request.getErrorPages(request.getStatusCode()));;
      return false;
  }
  return true;
}

int find_method_uri(HTTPRequest &request, const std::string &line) {
  std::stringstream sstream(line);
  //std::cout << "line ===> " << line << std::endl;
  // exit(99);
  std::string method, uri, httpVersion;
  sstream >> method >> uri >> httpVersion;
  if(!checkRequestURI(request, uri)) {
    //std::cout << "URI is not correct" << std::endl;
    return -1;
  }
  request.setLocation(uri);
  size_t queryPos = uri.find('?');
  if (queryPos != std::string::npos) {
      request.setQuery(uri.substr(queryPos + 1));
      request.setPath(uri.substr(0, queryPos));
      uri = uri.substr(0, queryPos);
  }
  if(uri[uri.length() - 1] == '/' && uri.size() == 1) {
    uri = uri.substr(0, uri.length() - 1);
    ////std::cout << "new path ==> " << (uri.empty() ? "only slash" : uri) << std::endl;
    ////std::cout << "|" << uri << "|" << std::endl;
    uri = "/";
  } else {
    size_t size = uri.size() - 1;
    if (uri[size] == '/')
      size--;
      // ! -------------------------- !/ Exception thrown ?
    uri = uri.substr(1, size);
    ////std::cout << "uri +++ " << uri << std::endl;
    // exit(0);
  }
  request.setMethod(method);
  if(uri == "...")
    request.setPath("index.html");
  else
    request.setPath(uri);
  //std::cout << "\n\n\nhandle request : " << request.getPath() << "uri: " << uri << "\n\n\n" << std::endl;

  // //std::cout << "get path ---> " << request.getPath() << std::endl;
  // exit(99);
  request.setHTTPVersion(httpVersion);
  return 1;
}

std::vector<char> trim_crlf(const std::string &str) {
    size_t end = str.size();
    std::vector<char> vec;
    while (end > 0 && (str[end - 1] == '\r' || str[end - 1] == '\n')) {
        --end;
    }
    for(size_t i = 0; i < end; i++) {
      vec.push_back(str[i]);
    }
    // return str.substr(0, end);
    return vec;
}

// std::string trim(const std::string& str) {
//     std::string::size_type start = 0;
//     while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
//       ++start;
//     }
//     std::string::size_type end = str.size();
//     while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
//         --end;
//     }
//     return str.substr(start, end - start);
// }

std::vector<FormFile> parseMultipartFormData(const std::vector<char> &body, const std::string &boundary) {
    //std::cout << "boundary: " << boundary << std::endl;
    std::vector<FormFile> files;
    std::string boundaryMarker = "--" + boundary;
    std::string endMarker = boundaryMarker + "--";
    std::vector<char>::const_iterator pos = body.begin();
    while (pos != body.end()) {
        std::vector<char>::const_iterator partStart = std::search(pos, body.end(), boundaryMarker.begin(), boundaryMarker.end());
        if (partStart == body.end())
            break;
        partStart += boundaryMarker.size();
        if ((body.end() - partStart) >= 2 && partStart[0] == '-' && partStart[1] == '-')
            break;
        if ((body.end() - partStart) >= 2 && partStart[0] == '\r' && partStart[1] == '\n')
            partStart += 2;
        std::vector<char>::const_iterator headerEnd = std::search(partStart, body.end(), 
                                                                  "\r\n\r\n", "\r\n\r\n" + 4);
        if (headerEnd == body.end())
            break;
        std::string headerStr(partStart, headerEnd);
        std::istringstream hss(headerStr);
        std::string line;
        FormFile file;
        while (std::getline(hss, line)) {
            if (!line.empty() && line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            if (line.find("Content-Disposition:") != std::string::npos) {
                size_t namePos = line.find("name=\"");
                if (namePos != std::string::npos) {
                    size_t nameEnd = line.find("\"", namePos + 6);
                    if (nameEnd != std::string::npos)
                        file.name = line.substr(namePos + 6, nameEnd - (namePos + 6));
                }
                size_t filenamePos = line.find("filename=\"");
                if (filenamePos != std::string::npos) {
                    size_t filenameEnd = line.find("\"", filenamePos + 10);
                    if (filenameEnd != std::string::npos)
                        file.filename = line.substr(filenamePos + 10, filenameEnd - (filenamePos + 10));
                }
            } else if (line.find("Content-Type:") != std::string::npos) {
                size_t typePos = line.find(":");
                if (typePos != std::string::npos) {
                    file.contentType = line.substr(typePos + 1);
                    // Trim leading space
                    if (!file.contentType.empty() && file.contentType[0] == ' ')
                        file.contentType = file.contentType.substr(1);
                }
            }
        }
        std::vector<char>::const_iterator contentStart = headerEnd + 4;
        std::vector<char>::const_iterator nextBoundary = std::search(contentStart, body.end(), 
                                                                      boundaryMarker.begin(), boundaryMarker.end());
        std::vector<char>::const_iterator contentEnd = nextBoundary;
        if (contentEnd >= body.begin() + 2 && *(contentEnd - 2) == '\r' && *(contentEnd - 1) == '\n') {
            contentEnd -= 2;
        }
        if (!file.filename.empty()) {
          contentEnd -= (boundary.size() + 3);
          file.data.assign(contentStart, contentEnd);
          files.push_back(file);
        }
        pos = nextBoundary;
    }
    return files;
}

std::string extractDirectory(const std::string& location)
{
    if (location.empty())
        return "";
    if (location == "/")
        return "/";
    size_t firstSlash = location.find('/');
    size_t lastSlash = location.rfind('/');
    size_t dot = location.find('.');

    if (lastSlash == firstSlash && dot != std::string::npos)
        return "/";
    if (lastSlash == firstSlash && dot == std::string::npos)
        return location;
    // if (dot != std::string::npos && lastSlash != firstSlash) {
    if (lastSlash != firstSlash) {
        size_t secondSlash = location.find('/', firstSlash + 1);
        if (secondSlash != std::string::npos)
            return location.substr(0, secondSlash);
    }
    if (lastSlash == location.length() - 1) {
        std::string trimmed = location;
        while (!trimmed.empty() && trimmed[trimmed.length() - 1] == '/') {
            trimmed.erase(trimmed.length() - 1);
        }
        return extractDirectory(trimmed);
    }
   //std::cout << "-------------> here" << std::endl;
    return location.substr(0, lastSlash);
}