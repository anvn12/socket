#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class WinsockManager final {
private:
	WSADATA wsaData;
public:
	WinsockManager() {}
	~WinsockManager() {}

	// no copy
	WinsockManager(const WinsockManager&) = delete;
	WinsockManager& operator=(const WinsockManager&) = delete;

	// Initialize Winsock
	bool init(); // false: cannot init
	// shutdown winsock
	void cleanup();
};
