#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class SocketClient final {
private:
	SOCKET socket_ = INVALID_SOCKET;

	bool isConnected = false;
	bool isQuit = false;
	vector<string> command;		// store command and arguments
	string serverIP;
	string username;
	string password;

	const char* port = "21";	// tcp


	bool connectToServer(addrinfo* result);
public:
	SocketClient() {}
	~SocketClient() {}

	void close();

	SOCKET& getSocket() { return socket_; }
	bool getIsConnected() const { return isConnected; }
	bool getIsQuit() const { return isQuit; }
	


	void inputCommand();
	bool processCommand();	// false: invalid command
	void getResponseMessage();
	void sendCommandMessage(const char*);
};


