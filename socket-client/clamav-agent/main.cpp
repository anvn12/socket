#include "WinsockManager.h"
#include "SocketServer.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


int main() {
	WinsockManager winsockManager;
	// Initialize Winsock
	if (winsockManager.init() == false) {
		return 1;
	}

	// Create a server for connecting to server (in result)
	SocketServer server;

	if (server.clamavBind() == false)
	{
		winsockManager.cleanup();
		return 1;
	}
	


	// cleanup Winsock
	server.close();
	winsockManager.cleanup();
	return 0;
}