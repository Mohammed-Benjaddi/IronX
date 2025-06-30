#include "../../headers/FileStreamer.hpp"

/**
 * the stat struct and fstat syscall to gether info about
 * a file descriptor abd validate its properties
 *  The struct stat is a data structure provided by the 
 * operating system to store metadata about a file. 
 * This includes information such as the file size, permissions,
 *  and type (e.g., regular file, directory, etc.). 
 *  fstat funciton syscall retrieving metadata about file associated
 * with given file descriptor _fd populating stat strcuture
*/

FileStreamer::FileStreamer(const std::string& path, std::string connection) :
     _path(path), _offset(0), _eof(false), _connectionHeader(connection), _rangeStart(0),
     _rangeEnd(0), _isPartial(true)
{   
   std::cout << "File Streamer - Opening file: " << path << std::endl;
    _fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (_fd == -1)
        throw FileFailure();

    struct stat st;
    if (fstat(_fd, &st) == -1 || !S_ISREG(st.st_mode))
        throw FileSyscall();
    _totalSize = st.st_size;
}

// FileStreamer::FileStreamer(const std::string& connection) 
//     : _path(""), _offset(0), _eof(true), _connectionHeader(connection), 
//       _rangeStart(0), _rangeEnd(0), _isPartial(false) , _fd(-1), _totalSize(0) {
// }

FileStreamer::~FileStreamer() { if (_fd != -1) close(_fd); }

bool FileStreamer::isOpen() const { return (fcntl(_fd, F_GETFD) != -1); };

int FileStreamer::getFD() const { return _fd; };

bool FileStreamer::isEOF() const { return (_eof); }

size_t  FileStreamer::getFileSize() {
    return _totalSize;
}

std::string FileStreamer::getNextChunk(size_t chunkSize) {
    if (_eof) return "";
    if (!isOpen()) throw EncodingError();

    // 8192 is a common buffer size chosen for efficiency, balancing memory usage and I/O performance.    
    char buffer[8192];

    //? Determine the number of bytes to read, which is the smaller of the chunk size or the buffer size (itll never go over 8192).

    ssize_t bytesToRead = std::min(chunkSize, sizeof(buffer));
    ssize_t bytesRead = pread(_fd, buffer ,bytesToRead, _offset); //? reading without changing f*
    if (bytesRead == 0) {
        _eof = true;
        return "";
    } else if (bytesRead < 0) {
        // ? Bdd fd
        return "";
    }
    _offset += bytesRead;
    if (_offset >= static_cast<off_t>(_totalSize))
        _eof = true;
    return std::string(buffer, bytesRead);
};

std::string FileStreamer::getConnectionHeader() {
    return _connectionHeader;
};

void    FileStreamer::setRange(size_t start, size_t end) {
    _rangeStart = start;
    _rangeEnd = std::min(end, _totalSize - 1);
    _offset = _rangeStart;
    _isPartial = true;
    _eof = false;
}

std::string FileStreamer::getPath() const {
    return _path;
}

size_t  FileStreamer::getStartByte() const {
    return _rangeStart;
}

size_t  FileStreamer::getEndByte() const {
    return _rangeEnd;
}

bool    FileStreamer::isPartialRequest() const {
    return _isPartial;
}

const char* FileStreamer::FileFailure::what() const throw () {
    return "FileStreamer - Couldn't open file for read\n";
}

const char* FileStreamer::FileSyscall::what() const throw () {
    return "FileStreamer - File Descriptor invdalid (FileStreamerClass)\n";
}

const char* FileStreamer::EncodingError::what() const throw () {
    return "FileStreamer - FD is closed /  \n";
}