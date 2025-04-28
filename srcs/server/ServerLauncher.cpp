#include "../../headers/ServerLauncher.hpp"

ServerLauncher::ServerLauncher() {};

ServerLauncher::~ServerLauncher() {};

void	ServerLauncher::launch(const WebServerConfig &config) {
	const	std::vector<Cluster>& clusters = config.getClusters();

	//? For each Cluster
	for (size_t i = 0; i < clusters.size(); ++i) {
		const Cluster& cluster = clusters.at(i);
		const std::string& host = cluster.getHost();
		std::cout << "Listening on host: " << host << std::endl;
		const std::vector<uint16_t>& ports = cluster.getPorts();
		//? For each Port
		for (size_t j = 0; j < ports.size(); ++j) {
			const std::vector<uint16_t> ports = cluster.getPorts();
			std::cout << "\t\tListening on port: " << ports.at(j) << std::endl;
		}
	}
}