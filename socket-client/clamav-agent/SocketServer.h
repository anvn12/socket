#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;




class SocketServer final {
private:
	SOCKET socket_ = INVALID_SOCKET;
	string serverIP;
	const char* port = "5555";	// custom port for clamav server

public:
	SocketServer() {}
	~SocketServer() {}

	void close();


	//	False: thất bại trong thực hiện chức năng
	//init socket_ and bind the ip
	bool clamavBind();

	//server listens for connection
	bool clamavListen();

	//server accepts the connection
	bool clamavAccept();


	void scan();


};

struct IPv4
{
	unsigned int b1, b2, b3, b4;
};
