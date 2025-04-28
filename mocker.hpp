#pragma once

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <iomanip>
#include "headers/WebServerConfig.hpp"

void printConfig(const WebServerConfig& config) {
    // Print global settings
    std::cout << "=== Global Settings ===\n";
    std::cout << "Max Body Size: " << config.getMaxBodySize() << " bytes\n\n";

    // Print error pages
    std::cout << "=== Error Pages ===\n";
    const std::map<int, std::string>& error_pages = config.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
        std::cout << it->first << ": " << it->second << "\n";
    }
    std::cout << "\n";

    // Print clusters (servers)
    const std::vector<Cluster>& clusters = config.getClusters();
    for (size_t i = 0; i < clusters.size(); i++) {
        const Cluster& cluster = clusters[i];
        std::cout << "=== Cluster #" << (i + 1) << " ===\n";
        std::cout << "Host: " << cluster.getHost() << "\n";

        // Print ports
        std::cout << "Ports: [";
        const std::vector<uint16_t>& ports = cluster.getPorts();
        for (size_t j = 0; j < ports.size(); j++) {
            if (j != 0) std::cout << ", ";
            std::cout << ports[j];
        }
        std::cout << "]\n";

        // Print hostnames
        std::cout << "Hostnames: [";
        const std::vector<std::string>& hostnames = cluster.getHostnames();
        for (size_t j = 0; j < hostnames.size(); j++) {
            if (j != 0) std::cout << ", ";
            std::cout << hostnames[j];
        }
        std::cout << "]\n\n";

        // Print routes
        std::cout << "=== Routes ===\n";
        const std::map<std::string, Route>& routes = cluster.getRoutes();
        for (std::map<std::string, Route>::const_iterator route_it = routes.begin();
            route_it != routes.end(); ++route_it) {
            const Route& route = route_it->second;
            std::cout << "  Route (" << route_it->first << "):\n";
            std::cout << "    Root Directory: " << route.getRootDir() << "\n";

            // Print index files
            const std::vector<std::string>& index_files = route.getIndexFiles();
            if (!index_files.empty()) {
                std::cout << "    Index Files: [";
                for (size_t j = 0; j < index_files.size(); j++) {
                    if (j != 0) std::cout << ", ";
                    std::cout << index_files[j];
                }
                std::cout << "]\n";
            }

            // Print allowed methods
            std::cout << "    Allowed Methods: [";
            const std::set<std::string>& methods = route.getAllowedMethods();
            for (std::set<std::string>::const_iterator method_it = methods.begin();
                method_it != methods.end(); ++method_it) {
                if (method_it != methods.begin()) std::cout << ", ";
                std::cout << *method_it;
            }
            std::cout << "]\n";

            // Print other route settings
            std::cout << "    Autoindex: " << (route.isAutoindex() ? "on" : "off") << "\n";
            if (!route.getRedirect().empty()) {
                std::cout << "    Redirect: " << route.getRedirect() << "\n";
            }
            if (!route.getUploadDir().empty()) {
                std::cout << "    Upload Directory: " << route.getUploadDir() << "\n";
            }

            // Print CGI config if exists
            const CGIConfig& cgi = route.getCGIConfig();
            const std::vector<std::string>& extensions = cgi.getExtensions();
            if (!extensions.empty()) {
                std::cout << "    CGI Configuration:\n";
                std::cout << "      Extensions: [";
                for (size_t j = 0; j < extensions.size(); j++) {
                    if (j != 0) std::cout << ", ";
                    std::cout << extensions[j];
                }
                std::cout << "]\n";
                std::cout << "      Interpreter: " << cgi.getInterpreter() << "\n";
            }

            std::cout << "\n";
        }
        std::cout << "\n";
    }
}



void mocker(WebServerConfig &wsc) {
    // Set global max body size
    wsc.setMaxBodySize(1048576);  // 1MB

    // Set default error pages
    std::map<int, std::string> error_pages;
    error_pages.insert(std::make_pair(400, "/errors/400.html"));
    error_pages.insert(std::make_pair(403, "/errors/403.html"));
    error_pages.insert(std::make_pair(404, "/errors/404.html"));
    error_pages.insert(std::make_pair(500, "/errors/500.html"));
    error_pages.insert(std::make_pair(503, "/errors/503.html"));
    wsc.setErrorPages(error_pages);

    // Create first server (cluster)
    Cluster server1;
    server1.setHost("0.0.0.0");
    
    std::vector<uint16_t> ports1;
    ports1.push_back(8080);
    ports1.push_back(4242);
    ports1.push_back(1923);
    server1.setPorts(ports1);
    
    std::vector<std::string> hostnames1;
    hostnames1.push_back("example.com");
    hostnames1.push_back("www.example.com");
    server1.setHostnames(hostnames1);

    // Create routes for first server
    std::map<std::string, Route> routes1;

    // Route 1: /
    Route route1;
    route1.setRootDir("/var/www/html");
    
    std::vector<std::string> indexFiles1;
    indexFiles1.push_back("index.html");
    route1.setIndexFiles(indexFiles1);
    
    std::set<std::string> methods1;
    methods1.insert("GET");
    methods1.insert("HEAD");
    route1.setAllowedMethods(methods1);
    
    route1.setAutoindex(false);
    routes1.insert(std::make_pair("/", route1));

    // Route 2: /cgi-bin
    Route route2;
    route2.setRootDir("/var/www/cgi-bin");
    
    std::set<std::string> methods2;
    methods2.insert("GET");
    methods2.insert("POST");
    route2.setAllowedMethods(methods2);
    
    CGIConfig cgi1;
    std::vector<std::string> extensions1;
    extensions1.push_back(".php");
    cgi1.setExtensions(extensions1);
    cgi1.setInterpreter("/usr/bin/php-cgi");
    route2.setCGIConfig(cgi1);
    
    routes1.insert(std::make_pair("/cgi-bin", route2));

    // Route 3: /uploads
    Route route3;
    route3.setRootDir("/var/www/uploads");
    
    std::set<std::string> methods3;
    methods3.insert("POST");
    route3.setAllowedMethods(methods3);
    
    route3.setUploadDir("/var/www/uploads/tmp");
    routes1.insert(std::make_pair("/uploads", route3));

    // Route 4: /old-site
    Route route4;
    route4.setRedirect("https://beta.example.com");
    
    std::set<std::string> methods4;
    methods4.insert("GET");
    route4.setAllowedMethods(methods4);
    
    routes1.insert(std::make_pair("/old-site", route4));

    server1.setRoutes(routes1);

    // Create second server (cluster)
    Cluster server2;
    server2.setHost("0.0.0.0");
    
    std::vector<uint16_t> ports2;
    ports2.push_back(8080);
    server2.setPorts(ports2);
    
    std::vector<std::string> hostnames2;
    hostnames2.push_back("api.example.com");
    hostnames2.push_back("beta.example.com");
    server2.setHostnames(hostnames2);

    // Create routes for second server
    std::map<std::string, Route> routes2;

    // Route 1: /
    Route route5;
    route5.setRootDir("/var/www/api");
    
    std::vector<std::string> indexFiles2;
    indexFiles2.push_back("api.php");
    route5.setIndexFiles(indexFiles2);
    
    std::set<std::string> methods5;
    methods5.insert("GET");
    methods5.insert("POST");
    methods5.insert("DELETE");
    route5.setAllowedMethods(methods5);
    
    routes2.insert(std::make_pair("/", route5));

    // Route 2: /cgi-bin
    Route route6;
    route6.setRootDir("/var/www/cgi-bin");
    
    std::set<std::string> methods6;
    methods6.insert("GET");
    methods6.insert("POST");
    route6.setAllowedMethods(methods6);
    
    routes2.insert(std::make_pair("/cgi-bin", route6));

    server2.setRoutes(routes2);

    // Add both servers to the WebServerConfig
    std::vector<Cluster> clusters;
    clusters.push_back(server1);
    clusters.push_back(server2);
    wsc.setClusters(clusters);
}