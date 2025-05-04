#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
// #include <cstdint>
#include <iostream>
#include <stdint.h>
#include <stdexcept>

//? CGI settings
class CGIConfig {
    private:
        std::vector<std::string> extensions;
        std::string interpreter;
    public:
        void setExtensions(const std::vector<std::string>& extensions);
        void setInterpreter(const std::string& interpreter);
        const std::vector<std::string>& getExtensions() const;
        const std::string& getInterpreter() const;
};

//? Route configuration
class Route {
    private:
        std::string root_dir;
        std::vector<std::string> index_files;
        std::set<std::string> allowed_methods;
        bool autoindex;
        std::string redirect;
        std::string upload_dir;
        CGIConfig cgi;
    public:
        void setRootDir(const std::string& root_dir);
        void setIndexFiles(const std::vector<std::string>& index_files);
        void setAllowedMethods(const std::set<std::string>& allowed_methods);
        void setAutoindex(bool autoindex);
        void setRedirect(const std::string& redirect);
        void setUploadDir(const std::string& upload_dir);
        void setCGIConfig(const CGIConfig& cgi);

        const std::string& getRootDir() const;
        const std::vector<std::string>& getIndexFiles() const;
        const std::set<std::string>& getAllowedMethods() const;
        bool isAutoindex() const;
        const std::string& getRedirect() const;
        const std::string& getUploadDir() const;
        const CGIConfig& getCGIConfig() const;
};

//? Cluster (server block) condifuration
class Cluster {
    private:
        std::string host;
        std::vector<uint16_t> ports;
        std::vector<std::string> hostnames;
        std::map<std::string, Route> routes;
    public:
        void setHost(const std::string& host);
        void setPorts(const std::vector<uint16_t>& ports);
        void setHostnames(const std::vector<std::string>& hostnames);
        void setRoutes(const std::map<std::string, Route>& routes);

        const std::string& getHost() const;
        const std::vector<uint16_t>& getPorts() const;
        const std::vector<std::string>& getHostnames() const;
        const std::map<std::string, Route>& getRoutes() const;
};

class WebServerConfig {
    private:
        size_t max_body_size;
        std::map<int, std::string> error_pages;
        std::vector<Cluster> clusters;
    public:
        void setMaxBodySize(size_t max_body_size);
        void setErrorPages(const std::map<int, std::string>& error_pages);
        void setClusters(const std::vector<Cluster>& clusters);

        size_t getMaxBodySize() const;
        const std::map<int, std::string>& getErrorPages() const;
        const std::vector<Cluster>& getClusters() const;
};
