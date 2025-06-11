#include "../../headers/responseUtils.hpp"

std::map<std::string, std::string> initMimeTypes() {
    std::map<std::string, std::string> m;
    m[".html"] = "text/html";
    m[".htm"]  = "text/html";
    m[".css"]  = "text/css";
    m[".js"]   = "application/javascript";
    m[".json"] = "application/json";
    m[".jpg"]  = "image/jpeg";
    m[".jpeg"] = "image/jpeg";
    m[".png"]  = "image/png";
    m[".gif"]  = "image/gif";
    m[".mp3"]  = "audio/mpeg";
    m[".mp4"]  = "video/mp4";
    m[".pdf"]  = "application/pdf";
    m[".txt"]  = "text/plain";
    return m;
}

size_t getFileSize(std::string path) {
    struct stat st;
    if (stat(path.c_str(), &st) == -1) {
        return 0;
    }
    return static_cast<size_t>(st.st_size);
}

std::string HTTPResponse::getMimeType(const std::string& path) {
    size_t dotPos = path.find_last_of(".");
    if (dotPos == std::string::npos)
        return "text/plain";

    std::string ext = path.substr(dotPos);
    if (_mimeTypes.count(ext))
        return _mimeTypes[ext];
    return "text/plain";
}

std::string toString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void build_OK_Response(HTTPRequest* req, HTTPResponse* res) {
    setStandardHeaders(res, res->getMimeType(req->getPath()), getFileSize(req->getPath()), "keep-alive" ,200, "OK");
    res->setHeader("Accept-Ranges",  "bytes");
    res->setStreamer(new FileStreamer(req->getPath(), res->getConnectionHeader()));
}

void buildResponse(HTTPRequest* req, HTTPResponse* res) {
    int status = req->getStatusCode();
    int size = getFileSize(req->getPath());

        switch (status) {
        case 404:
            setStandardHeaders(res, "text/html", size, "keep-alive", 404, "Not Found");
            res->setStreamer(new FileStreamer(req->getPath(), res->getConnectionHeader()));
            break;
        case 403:
            setStandardHeaders(res, "text/html", size , "close", 403, "Forbidden");
            res->setStreamer(new FileStreamer(req->getPath(), res->getConnectionHeader()));
            break;
        case 405:
            setStandardHeaders(res, "text/html", size, "close", 405, "Method Not Allowed");
            res->setStreamer(new FileStreamer(req->getPath(), res->getConnectionHeader()));
            break;
        case 9999:
            setStandardHeaders(res, "text/html", size, "close", 200, "OK");
            res->buildAutoIndexResponse(req);
            res->setStreamer(new FileStreamer(req->getPath(), res->getConnectionHeader()));
            break;
        default:
            setStandardHeaders(res, "text/html", size, "close", 500, "Internal Server Error");
    }
}

void setStandardHeaders(HTTPResponse* res, const std::string& contentType, 
    size_t contentLength, const std::string& connection, int statusCode, std::string statusMessage) {
    res->setHeader("Content-Type", contentType);
    res->setHeader("Content-Length", toString(contentLength));
    res->setHeader("Connection", connection);
    res->setStatus(statusCode, statusMessage);
}

std::string getRelativePath(const std::string& path, const std::string& rootPath) {
    std::string relative;
    if (path.find(rootPath) == 0) {
        relative = path.substr(rootPath.length()); 
    } else {
        relative = path;
    }

    relative = relative.substr(0, relative.size() - 11);

    std::cout << "relative path: " << relative << "\n";
    return relative;
}





// HTTPResponse fromRedirect(const std::string &location) {
//     HTTPResponse res;
//     res.setStatus(301, "Moved Permanently");
//     res.setHeader("Location", location);
//     res.setHeader("Content-Length", "0");
//     res.setHeader("Connection", "close");
//     return res;
// }

// HTTPResponse fromError(int code, const std::string& message, const std::string& body) {
//     HTTPResponse res;
//     res.setStatus(code, message);
//     res.setHeader("Content-Type", "text/html");
//     res.setHeader("Connection", "close");
//     res.setBody(body);
//     return res;
// }

// void buildAutoIndexResponse(HTTPResponse *res, std::string rootPath, std::string curPath) {
//     std::string directoryPath = curPath.substr(0, curPath.find_last_of("/\\"));
//     std::string templatePath = rootPath + "/autoindex.html";
//     std::string outputPath = directoryPath + "/index.html";

//     std::vector<std::string> entries = getDirectoryListing(directoryPath, false);

//     std::ifstream templateFile(templatePath.c_str());
//     if (!templateFile.is_open()) {
//         res->setStatus(500, "Internal Server Error");
//         res->setBody("<html><body><h1>500 Internal Server Error: Missing autoindex.html</h1></body></html>");
//         res->setHeader("Content-Type", "text/html");
//         return;
//     }

//     std::ostringstream beforeBody,afterBody, bodyRows;
    
//     std::string line;
//     bool insideTbody = false, tbodyFound = false;

//     while (std::getline(templateFile, line)) {
//         if (line.find("<tbody>") != std::string::npos) {
//             beforeBody << line << '\n';
//             insideTbody = true;
//             tbodyFound = true;
//             continue;
//         }
//         if (line.find("</tbody>") != std::string::npos) {
//             insideTbody = false;
//             afterBody << line << '\n';
//             break;
//         }
//         if (!insideTbody) {
//             beforeBody << line << '\n';
//         }
//     }

//     if (!tbodyFound) {
//         res->setStatus(500, "Internal Server Error");
//         res->setBody("<html><body><h1>500 Internal Server Error: &lt;tbody&gt; not found in template</h1></body></html>");
//         res->setHeader("Content-Type", "text/html");
//         return;
//     }

//     for (std::vector<std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
//         const std::string& entry = *it;
//         std::string fullEntryPath = directoryPath + "/" + entry;

//         bodyRows << "        <tr>\n";
//         bodyRows << "          <td><a href=\"" << entry << "\">📂 " << entry << "</a></td>\n";
//         bodyRows << "          <td>-</td>\n";
//         bodyRows << "          <td>" << getFileSize(fullEntryPath) << " B</td>\n";
//         bodyRows << "        </tr>\n";
//     }

//     while (std::getline(templateFile, line)) {
//         afterBody << line << '\n';
//     }
//     templateFile.close();

//     std::string finalHTML = beforeBody.str() + bodyRows.str() + afterBody.str();

//     std::ofstream outputFile(outputPath.c_str());
//     if (!outputFile.is_open()) {
//         res->setStatus(500, "Internal Server Error");
//         res->setBody("<html><body><h1>500 Internal Server Error: Cannot write index.html</h1></body></html>");
//         res->setHeader("Content-Type", "text/html");
//         return;
//     }

//     outputFile << finalHTML;
//     outputFile.close();

//     // Set response
//     res->setStreamer(new FileStreamer(outputPath, res->getConnectionHeader()));
//     res->setStatus(200, "OK");
//     res->setHeader("Content-Type", "text/html");
//     res->setHeader("Content-Length", res->toString(res->getStreamer()->getFileSize()));
//     res->setHeader("Connection", "keep-alive");

//     // store to delete later
//     res->setTempFilePath(outputPath);
// }