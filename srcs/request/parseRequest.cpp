#include "parseRequest.hpp"


int checkAllowedMethods(HTTPRequest &request) {
  const std::map<std::string, Route>& routes = request.getConfig()->getClusters()[request.getClientId()].getRoutes();
  std::map<std::string, Route>::const_iterator route_it = routes.find(request.getPath());
  const std::set<std::string> &allowedMethod = route_it->second.getAllowedMethods();

  if(allowedMethod.find(request.getMethod()) == allowedMethod.end()) {
      request.setStatusCode(405);
      request.setStatusMessage("Method Not Allowed");
      return -1;
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

#include <unistd.h>
#include <string>

int parse( HTTPRequest &request, const std::string &raw_request) {
  std::string line;
  std::stringstream ss(raw_request);
  char buffer[BUFSIZ];

  if (getcwd(buffer, sizeof(buffer)) != NULL)
      request.setRootDir(std::string(buffer) + "/www/");
  else
      request.setRootDir("");

  // 413 error code if the request body larger than client max body in config file
  if(ss.str().length() > request.getConfig()->getMaxBodySize()) {
      request.setStatusCode(413);
      request.setStatusMessage("Request Entity Too Large");
      return -1;
  }
  std::getline(ss, line);
  find_method_uri(request, line);
  while (std::getline(ss, line))
    request.setHeaders(line);
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
  size_t queryPos = uri.find('?');
  if (queryPos != std::string::npos) {
      request.setQuery(uri.substr(queryPos + 1));
      request.setPath(uri.substr(0, queryPos));
  }
  if(uri[uri.length() - 1] == '/')
    uri = uri.substr(0, uri.length() - 1);
  request.setMethod(method);
  request.setPath(uri);
  request.setHTTPVersion(httpVersion);
}

std::string trim_crlf(const std::string &str) {
    size_t end = str.size();
    while (end > 0 && (str[end - 1] == '\r' || str[end - 1] == '\n')) {
        --end;
    }
    return str.substr(0, end);
}

std::vector<FormFile> parseMultipartFormData(const std::string &body, const std::string &boundary) {
    // std::cout << "-----------------------------------------" << std::endl;
    // std::cout << body << std::endl;
    // std::cout << "-----------------------------------------" << std::endl;

    std::cout << "boundary: " << boundary << std::endl;

    std::vector<FormFile> files;
    std::string delimiter = "--" + boundary;
    size_t pos = 0;
    size_t end = 0;

    while ((pos = body.find(delimiter, pos)) != std::string::npos) {
        pos += delimiter.length();
        if (body.substr(pos, 2) == "--") break;
        if (body[pos] == '\r') ++pos;
        if (body[pos] == '\n') ++pos;

        end = body.find("\r\n\r\n", pos);
        if (end == std::string::npos) break;

        // std::cout << "here ----------------->" << std::endl; 

        std::string header = body.substr(pos, end - pos);
        pos = end + 4;

        end = body.find(delimiter, pos);
        if (end == std::string::npos) break;

        std::string content = trim_crlf(body.substr(pos, end - pos));

        // std::cout << "content: " << content << " | header: " << header << std::endl;

        std::istringstream headerStream(header);
        std::string line;
        FormFile file;
        bool isFile = false;
        
        // std::cout << "here -----------------> " << header << std::endl; 
        // 
        while (std::getline(headerStream, line)) {
            if (line.find("Content-Disposition:") != std::string::npos) {
              std::cout << "line ----> " << line << std::endl;
                size_t namePos = line.find("name=\"");
                if (namePos != std::string::npos) {
                    namePos += 6;
                    size_t endName = line.find("\"", namePos);
                    file.name = line.substr(namePos, endName - namePos);
                }
                size_t filenamePos = line.find("filename=\"");
                if (filenamePos != std::string::npos) {
                    filenamePos += 10;
                    size_t endFilename = line.find("\"", filenamePos);
                    file.filename = line.substr(filenamePos, endFilename - filenamePos);
                    isFile = true;
                } else {
                  std::cout << "----------------> else executed" << std::endl;
                }
            } else if (line.find("Content-Type:") != std::string::npos) {
                size_t typePos = line.find(":");
                if (typePos != std::string::npos) {
                    file.contentType = line.substr(typePos + 1);
                    while (file.contentType.size() > 0 && file.contentType[0] == ' ')
                        file.contentType = file.contentType.substr(1);
                }
            }
        }

        if (isFile) {
            for(size_t i = 0; i < content.size(); i++)
                file.data.push_back(content[i]);
            files.push_back(file);
        }

        pos = end;
    }

    // std::cout << "form size ---> " << files.size() << std::endl;
    // for (size_t i = 0; i < files.size(); ++i) {
    //     std::cout << "File " << i + 1 << ":\n";
    //     std::cout << "  Name: " << files[i].name << "\n";
    //     std::cout << "  Filename: " << files[i].filename << "\n";
    //     std::cout << "  Content-Type: " << files[i].contentType << "\n";
    //     std::cout << "  Data size: " << files[i].data.size() << " bytes\n";
    //     // std::cout << "  Data preview: " << files[i].data.substr(0, 50) << "\n";
    //     std::cout << "Data preview: ";
    //     for(size_t j = 0; j < files[i].data.size(); j++) {
    //         std::cout << files[i].data[j];
    //     }
    //     std::cout << std::endl;
    // }

    return files;
}
