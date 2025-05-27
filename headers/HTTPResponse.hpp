#pragma once

#include "HTTPRequest.hpp"
#include "WebServerConfig.hpp"
#include "FileStreamer.hpp"

class FileStreamer;

//? Would build HTTP Request
class HTTPResponse {
    public:
        HTTPResponse(std::string connection, std::string path);
        std::string	getNextChunk();
        bool		isComplete() const;
        void		prepareHeaders();
        std::string getConnectionHeader();
    private:
        std::string     mimeType;
        std::string     _connection;
		FileStreamer    *_streamer;
		std::string	    _headers;
        std::string     _mimeType;
		bool            _headersSent;
		bool            _complete;

        static std::map<std::string, std::string> _mimeTypes;
        static std::map<std::string, std::string> initMimeTypes();
        static std::string getMimeType(const std::string& path);
};
