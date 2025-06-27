#include "SocketClient.h"

void SocketClient::close() {
	if (socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}


bool SocketClient::connectToServer(addrinfo* result) {
	int iResult{};		// for checking
	// attempt to connect to the server
	for (int num_attempt = 0; num_attempt < 10; ++num_attempt) {
		this->socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (this->socket_ == INVALID_SOCKET) { // fail to create socket
			std::cerr << "Socket failed: " << WSAGetLastError() << "\n";
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// connect to the server
		iResult = connect(this->socket_, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == 0) { // success
			break;
		}

		// fail to connect to the server
		std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
		closesocket(this->socket_);
		this->socket_ = INVALID_SOCKET;
	}

	if (socket_ == INVALID_SOCKET) { // fail to connect
		std::cerr << "Unable to connect to server!" << std::endl;
		return false;
	}

	return true;
}