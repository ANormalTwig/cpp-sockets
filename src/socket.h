#ifndef SOCKET_H
#define SOCKET_H

#include <memory>
#include <string>

#include <netdb.h>

class SocketClient {
	int sockfd;
	uint16_t svport;

	std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> address;

public:
	SocketClient(std::string host, uint16_t port);
	~SocketClient();

	uint16_t GetBoundPort();
	uint16_t GetServerPort();

	void Connect();
	void Write(std::string message);
	std::string Read(int bytes, int timeout);
};

#endif

