#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

#define CHUNK_SIZE 4096 // bytes


class SocketClient final {
private:
	SOCKET socket_ = INVALID_SOCKET;

	bool isConnected = false;
	bool isQuit = false;
	vector<string> command;		// store command and arguments
	string serverIP;
	string username;
	string password;

	const char* port = "21";	// ftp
	const char* clamavPort = "55555";


	//bool connectToServer(addrinfo* result);
	SOCKET createConnection(const string& ip, const string& port, bool withRetry = false);
	SOCKET createDataConnection(const string& dataIP, const string& dataPort);

	//for port mode
	SOCKET createListeningSocket(string& localIP, int& localPort);
	string formatPORTCommand(const string& ip, int port);


	string getResponseMessage();
	string getResponseMessage(SOCKET& s);
	void sendCommandMessage(const char*);
	void sendCommandMessage(SOCKET& s, const char* msg);
public:
	SocketClient() {}
	~SocketClient() {}

	void close();

	SOCKET& getSocket() { return socket_; }
	bool getIsConnected() const { return isConnected; }
	bool getIsQuit() const { return isQuit; }
	


	void inputCommand();
	bool processCommand();	// false: invalid command
};


