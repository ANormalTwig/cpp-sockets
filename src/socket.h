#ifndef SOCKET_H
#define SOCKET_H

#include <memory>
#include <string>

#include <netdb.h>

class SocketClient {
	int sockfd;

	std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> address;

public:
	SocketClient(std::string host, std::string port);
	~SocketClient();

	void Connect();
	void Write(std::string message);
	std::string Read(int bytes, int timeout);
};

#endif

