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


	char type = 'A';	// ascii (A), binary (I)

	//bool connectToServer(addrinfo* result);

	//cai nay la de tao client active connection, co tac dung khoi tao CONTROL connection hoac PASV DATA connection
	//dung khi ket noi voi FTP server: open <ip>..., tạo data connection cho PASV nhu cai lenh ls cũ
	//tóm gọn nó là client-side connection dùng khi mình muốn chủ động kết nối với server
	SOCKET createConnection(const string& ip, const string& port, bool withRetry = false); 
	SOCKET createDataConnection(const string& dataIP, const string& dataPort);

	//for port 
	//ngược lại với cái trên, cái này dùng khi mình cần chấp nhận 1 kết nối từ FTP Server
	//client khởi tạo 1 listening socket để chấp nhận DATA connection từ server.
	//nó chỉ khởi tạo cái listening thôi, xong nó sẽ chờ FTP server kết nối
	SOCKET createListeningSocket(string& localIP, int& localPort);
	string formatPORTCommand(const string& ip, int port);


	string getResponseMessage();
	string getResponseMessage(SOCKET& s);
	void sendCommandMessage(const char*);
	void sendCommandMessage(SOCKET& s, const char* msg);


	void put1File(const string& filePath);
	void put1FileASCII(const string& filePath);


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


