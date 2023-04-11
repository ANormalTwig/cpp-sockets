#ifndef SOCKET_H
#define SOCKET_H

#include <memory>
#include <string>

#include <netdb.h>
#include <sys/poll.h>

namespace Socket {

class TCPBase {
protected:
	int sockfd;
	pollfd readpoll;

	sockaddr address;
	socklen_t address_length;

	TCPBase(std::string address, uint16_t port, int flags = 0);
	TCPBase(int sockfd, sockaddr address, socklen_t address_length);
	~TCPBase();

public:
	uint16_t GetBoundPort();
};

struct Client: TCPBase {
protected:
	uint16_t svport;

public:
	Client(std::string host, uint16_t port);
	Client(int sockfd, sockaddr address, socklen_t address_length);

	void Connect();

	uint16_t GetServerPort();

	void Write(std::string message);
	std::string Read(int bytes = 1024, int timeout = -1);
};

struct Server: TCPBase {
public:
	Server(std::string address, uint16_t port);

	void Listen(int queue_max = 512);
	std::shared_ptr<Client> Accept(int timeout = -1);
};

} // namespace Socket

#endif

