#include <cstdio>
#include <iostream>
#include <vector>

#include "socket.h"

#define print(msg) std::cout << (msg) << std::endl
#define SERVER_PORT 8001

int main() {
	Socket::Server myServer("127.0.0.1", SERVER_PORT);
	myServer.Listen(1024);

	Socket::Client myClient("127.0.0.1", SERVER_PORT);
	myClient.Connect();

	std::shared_ptr<Socket::Client> client(myServer.Accept());

	myClient.Write("The J");

	print(client->Read(1024));

	return EXIT_SUCCESS;
}

