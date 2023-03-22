#include <cstdio>
#include <iostream>

#include "socket.h"

int main() {
	SocketClient mySocket("google.com", 443);
	mySocket.Connect();

	mySocket.Write("Please be patient, I have C++ syndrome.");

	std::cout << mySocket.Read(1024, -1) << std::endl;

	return EXIT_SUCCESS;
}

