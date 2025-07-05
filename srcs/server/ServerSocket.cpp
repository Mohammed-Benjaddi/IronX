#include "../../headers/ServerSocket.hpp"

/**
 * Create a TCP server socket using socket()
 * making it non blocking with fcntl()
 * binding it to a host/port
 * start listening for incoming conenctions
*/
ServerSocket::ServerSocket(const std::string& ip, uint16_t port, int clusterId) : _fd(-1), _host(ip), _port(port), _clientId(clusterId) {
	//? 1. Create a socket
		this->create_sock();
	//? 2. Set socket to non-blocking
		this->non_block();
	//? 3. bind socket to host:port
		this->bind_socket();
	//? 4. start linstening
		this->init_listen();
};

ServerSocket::~ServerSocket() {
	// if (this->_fd != -1) {
	// 	close(this->_fd);
	// 	this->_fd = -1;
	// }
};

int		ServerSocket::getFd() const { return this->_fd; }

void	ServerSocket::setFd(int fd) { this->_fd = fd; };

std::string		ServerSocket::getIP() const {
	return this->_host;
}

int		ServerSocket::getClusterId() const {
	return _clientId;
}

void	ServerSocket::create_sock() {
	int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_fd == -1) {
		std::cerr << "Can't create sock for pair " << this->_host << ":" << this->_port << std::endl;
		throw SocketCreationFailure();
	};
	this->setFd(server_fd);
}

void	ServerSocket::non_block() {
	int flags = fcntl(this->_fd, F_GETFL, 0);

	if (flags == -1) {
		std::cerr << "fcntl Failure" << std::endl;
		close(this->_fd);
		throw std::runtime_error("Failure (Socket flag Retreival)");
	};

	if (fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "fcntl(F_SETFL) failed" << std::endl;

        close(this->_fd);
        throw std::runtime_error("Failed to set socket to non-blocking");
	}

	int opt = 1;

	if (this->_fd < 0 || setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		throw std::runtime_error("Port Socket reuse failure !");
	}
}

void	*ft_memset(void *b, int c, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		*(unsigned char *)((unsigned char*)b + i) = (unsigned char)(c);
		i++;
	}
	return (b);
}

void ServerSocket::bind_socket() {
	struct addrinfo hints;
	struct addrinfo* res = NULL;

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Only IPv4 (like inet_pton was doing)
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // For binding

	std::stringstream ss;
	ss << this->_port;
	std::string port_str = ss.str();

	int status = getaddrinfo(this->_host.c_str(), port_str.c_str(), &hints, &res);

	if (status != 0 || res == NULL) {
		close(this->_fd);
		throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(status)));
	}

	if (bind(this->_fd, res->ai_addr, res->ai_addrlen) == -1) {
		freeaddrinfo(res);
		close(this->_fd);
		throw std::runtime_error("bind() failed");
	}

	freeaddrinfo(res);
}

void	ServerSocket::init_listen() {
	const int backlog = 128; //* max num of pending connections (queue for pending connections)
	//* Serve calls accept() and takes the first waiting connection

	if (listen(this->_fd, backlog) == -1) {
		close(this->_fd);
		throw std::runtime_error("listen() failed");
	}
	////std::cout << "Socket is now listening on " << this->_host << ":" << this->_port << std::endl;
}

const char *ServerSocket::SocketCreationFailure::what() const throw() {
	return ("Failed to Create Socket.");
}