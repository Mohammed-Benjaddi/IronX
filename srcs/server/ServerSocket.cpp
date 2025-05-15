#include "../../headers/ServerSocket.hpp"

/**
 * Create a TCP server socket using socket()
 * making it non blocking with fcntl()
 * binding it to a host/port
 * start listening for incoming conenctions
*/
ServerSocket::ServerSocket(const std::string& ip, uint16_t port) : _fd(-1), _host(ip), _port(port)  {
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
	// 	std::cout << "Server Socket Destroyed due to failure of one!" << std::endl;
	// 	close(_fd);
	// }
};

int		ServerSocket::getFd() const { return this->_fd; }

void	ServerSocket::setFd(int fd) { this->_fd = fd; };

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
		perror("fcntl Failure");
		close(this->_fd);
		throw std::runtime_error("Failure (Socket flag Retreival)");
	};

	if (fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl(F_SETFL) failed");
        close(this->_fd);
        throw std::runtime_error("Failed to set socket to non-blocking");
	}

}
//! to be handled ---- failure of one 
void	ServerSocket::bind_socket() {
//? 1. Set up address structure
	struct sockaddr_in addr;
	memset(&addr, 0 , sizeof(addr));
	addr.sin_family = AF_INET;
	//* Converting a string ip to bnf using inet_pton and setting s_addr
	if (inet_pton(AF_INET, this->_host.c_str(), &addr.sin_addr) != 1) {
    	close(this->_fd);
		throw std::runtime_error("Invalid IP Address - Check Network Config");
	}

	addr.sin_port = htons(this->_port); // port to network byte order
	//? 2. bind socket
	std::cout << "Attempting to bind " << this->_host << ":" << this->_port << std::endl;
	if (bind(this->_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		close(this->getFd());
		throw std::runtime_error("bind() failed");
	}
	std::cout << "Socket bound to port " << this->_port << std::endl;
}

void	ServerSocket::init_listen() {
	const int backlog = 128; //* max num of pending connections (queue for pending connections)
	//* Serve calls accept() and takes the first waiting connection

	if (listen(this->_fd, backlog) == -1) {
		close(this->_fd);
		throw std::runtime_error("listen() failed");
	}
	std::cout << "Socket is now listening on " << this->_host << ":" << this->_port << std::endl;
}

const char *ServerSocket::SocketCreationFailure::what() const throw() {
	return ("Failed to Create Socket.");
}