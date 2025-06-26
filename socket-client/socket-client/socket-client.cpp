#include "Header.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

//#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_PORT "21"
string serverIP = "192.168.157.1";


int main() {
	WSADATA wsaData;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) { // fail to start up
		std::cerr << "WSAStartup failed: " << iResult << std::endl;
		return 1;
	}

	struct addrinfo* result = NULL,
		//*ptr = NULL,
		hints;		//hints: give the supported type

	memset(&hints, 0, sizeof(hints));
	//IPV4 and TCP
	hints.ai_family = AF_INET;			//ipv4
	hints.ai_socktype = SOCK_STREAM;	//tcp protocol
	hints.ai_protocol = IPPROTO_TCP;	//tcp protocol

	// get the server address and port, put to result
	iResult = getaddrinfo(serverIP.c_str(), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) { //fail to get address
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return 1;
	}


	// Create a SOCKET for connecting to server (in result)
	SOCKET Sending_socket = INVALID_SOCKET;
	
	// attempt to connect to the server
	for (int num_attempt = 0; num_attempt < 10; ++num_attempt) {
		Sending_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (Sending_socket == INVALID_SOCKET) { // fail to create socket
			std::cerr << "Socket failed: " << WSAGetLastError() << "\n";
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// connect to the server
		iResult = connect(Sending_socket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == 0) { // success
			break;
		}

		// fail to connect to the server
		std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
		closesocket(Sending_socket);
		Sending_socket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (Sending_socket == INVALID_SOCKET) { // fail to connect
		std::cerr << "Unable to connect to server!" << std::endl;
		WSACleanup();
		return 1;
	}

	//iResult = 1;
	std::cout << "Connected to " << serverIP << endl;




	

	// cleanup Winsock
	closesocket(Sending_socket);
	WSACleanup();
	return 0;
}