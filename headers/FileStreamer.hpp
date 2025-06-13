#pragma once

#include <sstream>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

//? Class for streaming chunk by chunk file
    //? Opens file - Track File Position - Read Next chunk - Provide isEOF() - Error State

//? using fstat to get file information using its _fd

class HTTPResponse;

class FileStreamer {
    private:
        std::string _path;
        off_t       _offset;
        size_t      _totalSize;
        bool        _eof;
        int         _fd;
        std::string _connectionHeader;
        size_t      _rangeStart;
        size_t      _rangeEnd;
        bool        _isPartial;
    public:
        FileStreamer(const std::string&, std::string);
        // FileStreamer(const std::string&);
        // FileStreamer(HTTPRequest &);
        ~FileStreamer();

        std::string getNextChunk(size_t chunkSize = 8192);
        std::string getConnectionHeader();
        std::string getPath() const;
        size_t      getFileSize();
        size_t      getFileSize(std::string) const;
        bool        isEOF() const;
        bool        isOpen() const;
        int         getFD() const;
        void        setRange(size_t start, size_t end);
        size_t      getStartByte() const;
        size_t      getEndByte() const;
        bool        isPartialRequest() const;
        class FileFailure : public std::exception {
            public:
                const char* what() const throw();
        };
        class FileSyscall : public std::exception {
            public:
                const char* what() const throw();
        };
        class EncodingError : public std::exception {
            public:
                const char* what() const throw();
        };
};