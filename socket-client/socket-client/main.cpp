#include "Header.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string.h>


#include "WinsockManager.h"
#include "SocketClient.h"

//#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_PORT "21"
string serverIP = "192.168.157.1";


int main() {
	WinsockManager winsockManager;
	// Initialize Winsock
	if (winsockManager.init() == false) {
		return 1;
	}

	int iResult{}; // for checking 

	//hints: give the supported type (IPV4 TCP)
	addrinfo *result = NULL, hints;		
	memset(&hints, 0, sizeof(hints));	//clear all data of hints
	//IPV4 and TCP
	hints.ai_family = AF_INET;			//ipv4
	hints.ai_socktype = SOCK_STREAM;	//tcp protocol
	hints.ai_protocol = IPPROTO_TCP;	//tcp protocol


	// TODO:
	// input IP========================================




	// get the server address and port, put to result
	iResult = getaddrinfo(serverIP.c_str(), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) { //fail to get address
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		winsockManager.cleanup();
		return 1;
	}


	// Create a SOCKET for connecting to server (in result)
	SocketClient socketClient;

	// connect
	if (socketClient.connectToServer(result) == false) {
		freeaddrinfo(result);
		winsockManager.cleanup();
		return 1;
	}
	// connected
	std::cout << "Connected to " << serverIP << endl;
	freeaddrinfo(result);

	

	// get response message
	char message[1025];		// leave 1 char for '\0'

	iResult = recv(socketClient.getSocket(), message, sizeof(message) - 1, 0);
	
	if (iResult > 0) {
		message[iResult] = '\0';
	}

	cout << message;

	system("pause");


	// cleanup Winsock
	socketClient.close();
	winsockManager.cleanup();
	return 0;
}