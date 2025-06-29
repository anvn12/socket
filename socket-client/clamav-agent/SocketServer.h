#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class SocketServer final {
private:
	SOCKET socket_ = INVALID_SOCKET;

	//bool isConnected = false;

	string serverIP;
	const char* port = "5555";	// custom port for clamav server

public:
	SocketServer() {}
	~SocketServer() {}

	void close();

	//init socket_ and bind the ip
	bool clamavBind();
	bool clamavListen();

};


