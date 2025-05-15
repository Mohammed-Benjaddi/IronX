#include "../../headers/ServerLauncher.hpp"
#include "../../headers/ServerSocket.hpp"

ServerLauncher::ServerLauncher() {};

ServerLauncher::~ServerLauncher() {
	for (size_t i = 0; i < this->_sockets.size(); ++i) {
		close(this->_sockets.at(i).getFd());
	}
};

void	ServerLauncher::launch(const WebServerConfig &config) {
	const	std::vector<Cluster>& clusters = config.getClusters();
	std::vector<ServerSocket> sockets;
	//? For each Cluster
	for (size_t i = 0; i < clusters.size(); ++i) {
		const Cluster& cluster = clusters.at(i);
		const std::string& host = cluster.getHost();
		const std::vector<uint16_t>& ports = cluster.getPorts();
		//? For each Port
		for (size_t j = 0; j < ports.size(); ++j) {
			const std::vector<uint16_t> ports = cluster.getPorts();
			ServerSocket server_socket(host, ports.at(j));
			sockets.push_back(server_socket);
		}
	}
	this->_sockets = sockets;
}

const	std::vector<ServerSocket>&	ServerLauncher::getSockets() {
	return this->_sockets;
};