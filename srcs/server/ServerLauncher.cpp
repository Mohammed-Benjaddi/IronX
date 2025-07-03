#include "../../headers/ServerLauncher.hpp"
#include "../../headers/ServerSocket.hpp"
#include <sstream>
#include <iostream>   // for std::cerr, std::cout
#include <fstream>    // for std::ofstream  // for std::ostringstream
#include <vector>
#include <string>

ServerLauncher::ServerLauncher() {};

ServerLauncher::~ServerLauncher() {
	for (size_t i = 0; i < this->_sockets.size(); ++i) {
		close(this->_sockets.at(i).getFd());
	}
};

void printHostnames(std::vector<std::string> hostnames) {
	for (size_t i = 0; i < hostnames.size(); ++i) {
		std::cout << "Hostname: " << hostnames.at(i) << std::endl;
	}
	std::cout << std::endl;
}

void ServerLauncher::launch(const WebServerConfig &config) {
    const std::vector<Cluster>& clusters = config.getClusters();
    std::ostringstream hostsStream;

    for (const auto& cluster : clusters) {
        const std::string& ip = cluster.getHost();
        const std::vector<std::string>& hostnames = cluster.getHostnames();

        for (const auto& hostname : hostnames) {
            hostsStream << ip << " " << hostname << "\n";
        }

        const std::vector<uint16_t>& ports = cluster.getPorts();
        for (uint16_t port : ports) {
            ServerSocket serverSocket(ip, port, /* cluster id if needed */ 0);
            _sockets.push_back(serverSocket);
        }
    }

    // Append the collected host entries to the known_hosts file
    std::ofstream knownHostsFile("/etc/hosts", std::ios::app);
    if (!knownHostsFile.is_open()) {
        std::cerr << "Failed to open known_hosts file for appending.\n";
        return;
    }

    knownHostsFile << hostsStream.str();
    knownHostsFile.close();
}

const	std::vector<ServerSocket>&	ServerLauncher::getSockets() {
	return this->_sockets;
};