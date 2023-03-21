#include <iostream>
#include <string>

#include "socket.h"

int main() {
	SocketClient mySocket("localhost", "8001");

	mySocket.Connect();

	mySocket.Write("I'll kill you.");

	return EXIT_SUCCESS;
}

