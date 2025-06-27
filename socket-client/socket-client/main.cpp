#include "WinsockManager.h"
#include "SocketClient.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


int main() {
	WinsockManager winsockManager;
	// Initialize Winsock
	if (winsockManager.init() == false) {
		return 1;
	}

	// Create a SOCKET for connecting to server (in result)
	SocketClient client;


	while (client.getIsQuit() == false)
	{
		cout << "FTP>";
		client.inputCommand();
		if (client.processCommand() == false)
		{
			if (client.getIsQuit() == true)
			{
				break;
			}
			else  // invalid command, not quit
			{
				cout << "Invalid command.\n";
			}
		}

	}


	// cleanup Winsock
	client.close();
	winsockManager.cleanup();
	return 0;
}