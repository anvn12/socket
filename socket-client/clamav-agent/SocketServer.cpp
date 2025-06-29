#include "SocketServer.h"

void SocketServer::close() {
	if (socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}



bool SocketServer::clamavBind() {
	//get local ip
	char hostName[256];
	if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) {
		cerr << "Failed to get hostname: " << WSAGetLastError() << "\n";
		closesocket(socket_);
		return false;
	}

	//khoi tao hints cho dinh dang server: tcp, ipv4
	addrinfo hints, *result = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	//for checking
	int iResult{};	


	//truyen du lieu cho result
	iResult = getaddrinfo(hostName, this->port, &hints, &result);
	if (iResult != 0) {
		cerr << "getaddrinfo failed: " << iResult << endl;
		closesocket(socket_);
		return INVALID_SOCKET;
	}

	//init socket_
	socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (socket_ == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		return false;
	}

	// Setup the listening socket
	//bind: gàn buộc IP cho socket.
	iResult = bind(this->socket_, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		close();
		return false;
	}

	freeaddrinfo(result);

	return true;
}

bool SocketServer::clamavListen()
{
	if (listen(socket_, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		close();
		return false;
	}
	return true;
}