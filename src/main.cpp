#include <cstdio>
#include <iostream>

#include "socket.h"

int main() {
	SocketClient mySocket("google.com", 443);
	mySocket.Connect();

	mySocket.Write("Please be patient, I have C++ syndrome.");

	return EXIT_SUCCESS;
}

