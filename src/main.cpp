#include <cstdio>
#include <iostream>
#include <vector>

#include "socket.h"

#define print(msg) std::cout << (msg) << std::endl
#define SERVER_PORT 8001

int main() {
	Socket::Server myServer("127.0.0.1", SERVER_PORT);
	myServer.Listen(1024);

	std::vector<std::shared_ptr<Socket::Client>> clients;

	clients.push_back(myServer.Accept());

	print(clients.back()->Read(1024));

	return EXIT_SUCCESS;
}

