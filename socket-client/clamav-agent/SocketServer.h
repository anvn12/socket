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
	SOCKET clientSocket_ = INVALID_SOCKET;	//socket dùng để tương tác với client
	SOCKET listenSocket_ = INVALID_SOCKET;	//socket dùng để bind, listen
	string serverIP;
	//const char* port = "55555";	// custom port for clamav server
	unsigned int port = 55555;	// custom port for clamav server


	string clamscanSource;


	string getResponseMessage(SOCKET& s);
	void sendCommandMessage(SOCKET& s, const char*);


public:
	SocketServer() {}
	~SocketServer() {}

	void close();


	bool clamavInput();

	//	False: thất bại trong thực hiện chức năng
	//init listenSocket_ and bind the ip
	bool clamavBind();

	//server listens for connection
	bool clamavListen();

	//server accepts the connection
	bool clamavAccept();


	void scan();

};

