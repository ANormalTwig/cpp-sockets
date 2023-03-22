#include <string>
#include <memory>
#include <stdexcept>

#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "socket.h"

SocketClient::SocketClient(std::string host, uint16_t port): address(nullptr, &freeaddrinfo), svport(port) {
	addrinfo* tmp;
	int addr_err = getaddrinfo(host.c_str(), std::to_string(port).c_str(), 0, &tmp);
	this->address.reset(tmp);
	if (addr_err != 0) {
		throw std::runtime_error("Failed to get address info.");
	}

	this->sockfd = socket(this->address->ai_family, this->address->ai_socktype, 0);
	if (sockfd < 0) {
		throw std::runtime_error("Failed to create socket.");
	}

	this->pollinfo.fd = this->sockfd;
	this->pollinfo.events |= POLLIN;
}

SocketClient::~SocketClient() {
	close(this->sockfd);
}

uint16_t SocketClient::GetBoundPort() {
	sockaddr_in socket_address;
	static const int addr_size = sizeof(sockaddr_in);

	if (getsockname(this->sockfd, (sockaddr*)&socket_address, (socklen_t*)&addr_size) < 0) {
		throw std::runtime_error("Failed to fetch socket info.");
	}

	return socket_address.sin_port;
}

uint16_t SocketClient::GetServerPort() {
	return this->svport;
}

void SocketClient::Connect() {
	int errcode = connect(this->sockfd, this->address->ai_addr, this->address->ai_addrlen);
	if (errcode != 0) {
		throw std::runtime_error("Failed to connect socket.");
	}
}

void SocketClient::Write(std::string message) {
	int sent = send(this->sockfd, message.c_str(), message.length(), 0);
	if (sent < 0) {
		throw std::runtime_error("Failed to send message.");
	}
}

std::string SocketClient::Read(int bytes, int timeout) {
	int poll_result = poll(&this->pollinfo, nfds_t(1), timeout);
	if (poll_result == 0) {
		return std::string();
	}

	std::unique_ptr<char> buffer(new char[bytes]);
	read(this->sockfd, buffer.get(), bytes);

	return std::string(buffer.get());
}

