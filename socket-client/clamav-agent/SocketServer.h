#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include <fstream>
#include <string>


#pragma comment(lib, "Ws2_32.lib")
using namespace std;




class SocketServer final {
private:
	SOCKET socket_ = INVALID_SOCKET;
	SOCKET listenSocket_ = INVALID_SOCKET;
	string serverIP;
	//const char* port = "55555";	// custom port for clamav server
	unsigned int port = 55555;	// custom port for clamav server

	string getResponseMessage(SOCKET& s);
	void sendCommandMessage(SOCKET& s, const char*);
public:
	SocketServer() {}
	~SocketServer() {}

	void socketShutdown();
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

