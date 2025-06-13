#pragma once

#include <map>
#include <string>
#include <sstream>
#include <filesystem>
#include "FileStreamer.hpp"
#include "HTTPRequest.hpp"
#include "requestUtils.hpp"
#include "responseUtils.hpp"

class FileStreamer;
class HTTPRequest;

class HTTPResponse {
public:
    // Constructors
    HTTPResponse();
    HTTPResponse(HTTPRequest* request);
    ~HTTPResponse();

    std::string getNextChunk();
    bool isComplete() const;
    std::string getConnectionHeader();
    
    void    handleDeletion();

    size_t  getFileSize(std::string path) const;
    void    setStatus(int code, const std::string& message);
    void    setHeader(const std::string& key, const std::string& value);
    void    setConnection(const std::string& connection);

    void    setBody(const std::string& body);              
    void    setStreamer(FileStreamer* streamer);           

    int     getStatusCode() const;
    std::string getStatusMessage() const;
    std::map<std::string, std::string> getHeaders() const;

    // void build_OK_Response(HTTPRequest* request);
    // void buildResponse(HTTPRequest* request);          
    void buildAutoIndexResponse(HTTPRequest*);

    static std::string getMimeType(const std::string& path);

private:
    int                             _statusCode;
    std::string                     _statusMessage;
    std::string                     _connectionType;
    std::string                     _connection;

    FileStreamer*                   _streamer;
    HTTPRequest*                    _request;
    std::string                     _body;
    std::string                     _tempFilePath;
    size_t                          _bodyPos;

    std::map<std::string, std::string> _headers;

    bool                            _headersSent;
    bool                            _complete;

    // MIME types
    static std::map<std::string, std::string> _mimeTypes;
    // static std::map<std::string, std::string> initMimeTypes();
};

