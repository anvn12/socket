#ifndef SocketClient_h
#define SocketClient_h
#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class SocketClient final {
private:
	SOCKET socket_ = INVALID_SOCKET;
public:
	SocketClient() {}
	~SocketClient() {}

	void close();


	SOCKET& getSocket() { return socket_; }

	bool connectToServer(addrinfo* result);
};


#endif#pragma once
