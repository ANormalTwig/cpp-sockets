#include <cstring>
#include <string>
#include <memory>
#include <stdexcept>

#include <fcntl.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "socket.h"

// TCP Base

Socket::TCPBase::TCPBase(std::string address, uint16_t port, int flags) {
	static const addrinfo hints{
		flags,
		AF_UNSPEC,
		SOCK_STREAM,
		IPPROTO_TCP,
	};

	// OoooOOooOO I love unsafe memory management OOooOOOooooO
	addrinfo* tmp;
	int addr_err = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &tmp);

	if (addr_err != 0) {
		freeaddrinfo(tmp);
		throw std::runtime_error("Failed to get address info. (" + std::string(gai_strerror(addr_err)) + ")");
	}

	this->address = *tmp->ai_addr;
	this->address_length = tmp->ai_addrlen;
	freeaddrinfo(tmp);

	int sockfd = socket(this->address.sa_family, SOCK_STREAM, 0);
	if (sockfd < 0) {
		throw std::runtime_error("Failed to create socket. (" + std::string(std::strerror(errno)) + ")");
	}

	static const int enable(1);
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		close(sockfd);
		throw std::runtime_error("Failed to set socket options. (" + std::string(std::strerror(errno)) + ")");
	}

	this->sockfd = sockfd;

	this->readpoll.fd = sockfd;
	this->readpoll.events = POLLIN;
}
Socket::TCPBase::TCPBase(int sockfd, sockaddr address, socklen_t address_length): sockfd(sockfd), address(address), address_length(address_length) {
	this->readpoll.fd = sockfd;
	this->readpoll.events = POLLIN;
}

Socket::TCPBase::~TCPBase() {
	close(this->sockfd);
}

uint16_t Socket::TCPBase::GetBoundPort() {
	sockaddr_in socket_address;
	static const int addr_size = sizeof(sockaddr_in);

	if (getsockname(this->sockfd, (sockaddr*)&socket_address, (socklen_t*)&addr_size) < 0) {
		throw std::runtime_error("Failed to get socket info. (" + std::string(std::strerror(errno)) + ")");
	}

	return socket_address.sin_port;
}

uint16_t Socket::Client::GetServerPort() {
	return this->svport;
}

// TCP Client

Socket::Client::Client(std::string host, uint16_t port): TCPBase(host, port, AI_CANONNAME | AI_ALL | AI_V4MAPPED), svport(port) {}
Socket::Client::Client(int sockfd, sockaddr address, socklen_t address_length): TCPBase(sockfd, address, address_length) {}

void Socket::Client::Connect() {
	int errcode = connect(this->sockfd, &this->address, this->address_length);
	if (errcode != 0) {
		throw std::runtime_error("Failed to connect socket. (" + std::string(std::strerror(errno)) + ")");
	}
}

void Socket::Client::Write(std::string message) {
	int sent = send(this->sockfd, message.c_str(), message.length(), 0);
	if (sent < 0) {
		throw std::runtime_error("Failed to send message. (" + std::string(std::strerror(errno)) + ")");
	}
}

std::string Socket::Client::Read(int bytes, int timeout) {
	int poll_result = poll(&this->readpoll, nfds_t(1), timeout);
	if (poll_result == 0) {
		return std::string();
	}

	char buffer[bytes];
	read(this->sockfd, &buffer, bytes);

	return std::string(buffer);
}

// TCP Server

Socket::Server::Server(std::string address, uint16_t port): TCPBase(address, port, AI_CANONNAME | AI_ALL | AI_V4MAPPED | AI_PASSIVE ) {
	if (bind(this->sockfd, &this->address, this->address_length) != 0) {
		throw std::runtime_error("Failed to bind socket. (" + std::string(std::strerror(errno)) + ")");
	}
}

void Socket::Server::Listen(int queue_max) {
	if (listen(this->sockfd, queue_max) != 0) {
		throw std::runtime_error("Socket listen failed. (" + std::string(std::strerror(errno)) + ")");
	}
}

std::shared_ptr<Socket::Client> Socket::Server::Accept(int timeout) {
	int poll_result = poll(&this->readpoll, this->sockfd, timeout);
	if (poll_result == 0) {
		return nullptr;
	}

	sockaddr address; socklen_t length;
	int clientfd = accept(this->sockfd, &address, &length);

	if (clientfd == -1) {
		throw std::runtime_error("Failed to accept socket. (" + std::string(std::strerror(errno)) + ")");
	}

	return std::shared_ptr<Socket::Client>(new Socket::Client(clientfd, address, length));
}

