#include "SocketServer.h"



//	Đóng socket
void SocketServer::close() {
	if (socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}


//	Init socket_ and bind the IP, port
bool SocketServer::clamavBind() {
	//	Lấy địa chỉ IP (IPV4) của máy host server để làm IP cho server
	//	Nếu biết trước địa chỉ IP thì gán trực tiếp cho serverIP và bỏ qua bước này
	int ipv4size = 32; // xxx.xxx.xxx.xxx + '\0'
	char *hostName = new char[ipv4size];
	if (gethostname(hostName, ipv4size) == SOCKET_ERROR) {
		cerr << "Failed to get hostname: " << WSAGetLastError() << "\n";
		close();
		return false;
	}

	//	Gán hostName vào serverIP để lưu trữ
	//xóa bộ nhớ không dùng đến của hostName
	serverIP = hostName;


	struct hostent* host = gethostbyname(hostName);
	if (host == NULL)
	{
		WSACleanup();
		return false;
	}

	IPv4 myIP;


	myIP.b1 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b1;
	myIP.b2 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b2;
	myIP.b3 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b3;
	myIP.b4 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b4;


	cout << myIP.b1 << myIP.b2 << myIP.b3 << myIP.b4;


	delete[] hostName;
	hostName = NULL;



	//  Khởi tạo hints cho định dạng server: tcp, ipv4
	addrinfo hints, *result = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	//int iResult = getaddrinfo(serverIP.c_str(), nullptr, &hints, &result);

	//cout << result->ai_addr;
	//cout << serverIP;












	//for checking
	int iResult{};	


	//truyen du lieu cho result
	iResult = getaddrinfo(serverIP.c_str(), this->port, &hints, &result);
	if (iResult != 0) {
		cerr << "getaddrinfo failed: " << iResult << endl;
		close();
		return false;
	}

	//init socket_
	socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (socket_ == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		return false;
	}

	// Setup the server socket
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
	//	Lắng nghe có yêu cầu kết nối nào không
	if (listen(socket_, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		close();
		return false;
	}
	return true;
}

bool SocketServer::clamavAccept()
{
	//	Nếu có yêu cầu kết nối thì đồng ý kết nối

	//	Tạo một socket tạm để đồng ý kết nối
	SOCKET clientSocket = INVALID_SOCKET;

	// Accept a client socket
	clientSocket = accept(socket_, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		close();
		return false;
	}

	//	Chấp nhận xong thì không cần server socket nữa
	close();

	//	Chuyển socket_ về clientSocket để thực hiện tao tác xử lý (nhận, truyền lệnh, phản hồi,...)
	socket_ = clientSocket;

	
	return true;
}

void SocketServer::scan()
{
#define DEFAULT_BUFLEN 512

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(socket_, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(socket_, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				close();
				return;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			close();
			return;
		}

	} while (iResult > 0);
}
