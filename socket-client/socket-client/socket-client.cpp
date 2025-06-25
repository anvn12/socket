#include "Header.h"

int main() {

	WSADATA wsaData;

	// Initialize Winsock
	const int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0) { // fail to start up
		printf("WSAStartup failed: %d\n", startupResult);
		return 1;
	}




	// cleanup Winsock
	WSACleanup();

	return 0;
}