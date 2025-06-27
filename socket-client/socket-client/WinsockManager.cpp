#include "WinsockManager.h"

bool WinsockManager::init() {
	int iResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	if (iResult != 0) { // fail to start up
		std::cerr << "WSAStartup failed: " << iResult << std::endl;
		return false;
	}
	return true;
}

void WinsockManager::cleanup() {
	WSACleanup();
}
