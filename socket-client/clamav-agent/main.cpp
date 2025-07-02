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



	// Vì client theo dạng tcp, ipv4 nên server cũng theo dạng tcp, ipv4


	// Create a server for connecting to server
	SocketServer server;

	// Nhập đường dẫn clamav
	if (server.clamavInput() == false)
	{
		server.close();
		winsockManager.cleanup();
		return 1;
	}

	 
	//	Khởi tạo, ràng buộc IP port
	if (server.clamavBind() == false)
	{
		server.close();
		winsockManager.cleanup();
		return 1;
	}

	//	Lắng nghe kết nối
	if (server.clamavListen() == false)
	{
		server.close();
		winsockManager.cleanup();
		return 1;
	}

	while (true)
	{
		// Chấp nhận kết nối (tức là khi có ai đó kết nối vô thì chấp nhận)
		if (server.clamavAccept() == false)
		{
			continue;
		}
	
		// clamscan
		server.scan();
	}

	server.close();
	// cleanup Winsock
	winsockManager.cleanup();
	return 0;
}