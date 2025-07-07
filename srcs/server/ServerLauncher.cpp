#include "../../headers/ServerLauncher.hpp"
#include "../../headers/ServerSocket.hpp"

ServerLauncher::ServerLauncher() {};

void ServerLauncher::buildHostnames(std::vector<std::string> &hostnames, const std::string &host, std::ofstream &conf) {
    if (hostnames.empty()) return;

    for (std::vector<std::string>::iterator it = hostnames.begin(); it != hostnames.end(); ++it) {
        std::cout << "Hostname: " << *it << "\n";
        conf << host << " " << *it << std::endl;
    }
}

ServerLauncher::~ServerLauncher() {
	for (size_t i = 0; i < this->_sockets.size(); ++i) {
		close(this->_sockets.at(i).getFd());
	}
};

#include <cstdlib>

void	ServerLauncher::launch(const WebServerConfig &config) {
	const	std::vector<Cluster>& clusters = config.getClusters();
	std::vector<ServerSocket> sockets;
	//? For each Cluster

	// std::ofstream conf("/etc/hosts", std::ios::out | std::ios::trunc);
    // if (!conf) {
    //     std::cerr << "Erreur : impossible d'ouvrir le fichier\n";
    //     return;
    // }

	// conf << "127.0.0.1 localhost\n";

	for (size_t i = 0; i < clusters.size(); i++) {
		std::vector<std::string> hostnames = clusters.at(i).getHostnames();

		// buildHostnames(hostnames, clusters.at(i).getHost(), conf);
		
		const Cluster& cluster = clusters.at(i);
		
		const std::vector<uint16_t>& ports = cluster.getPorts();
		
		std::string host(cluster.getHost());

		//? For each Port
		for (size_t j = 0; j < ports.size(); ++j) {
			ServerSocket server_socket(host, ports.at(j), i);
			sockets.push_back(server_socket);
		}
	}
	this->_sockets = sockets;
}

const	std::vector<ServerSocket>&	ServerLauncher::getSockets() {
	return this->_sockets;
};