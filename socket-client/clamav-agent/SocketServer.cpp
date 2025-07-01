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
	//	Lấy địa chỉ IP (IPV4) của máy host server (cái client đã nhập khi kết nối)
	//để làm IP cho server


	//init socket_
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//argument cuối có thể là 0: OS sẽ SOCK_STREAM -> TCP
	
	if (socket_ == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		return false;
	}


	//  Khởi tạo hints cho định dạng server: tcp, ipv4
	//	Ref: https://youtu.be/WDn-htpBlnU?si=HDgzazwC9anAq5T-
	sockaddr_in hints;
	memset(&hints, 0, sizeof(hints));
	hints.sin_family = AF_INET;
	hints.sin_port = htons(port);
	hints.sin_addr.S_un.S_addr = INADDR_ANY;	//để bind tới tất cả IP của máy


	//addrinfo hints, *result = NULL;
	//memset(&hints, 0, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;


	//for checking
	int iResult{};	
	iResult = bind(this->socket_, (sockaddr*)& hints, sizeof(hints));
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		close();
		return false;
	}

	//freeaddrinfo(result);

	////truyen du lieu cho result
	//iResult = getaddrinfo(serverIP.c_str(), this->port, &hints, &result);
	//if (iResult != 0) {
	//	cerr << "getaddrinfo failed: " << iResult << endl;
	//	close();
	//	return false;
	//}

	//socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	// Setup the server socket
	//bind: gàn buộc IP cho socket.
	//iResult = bind(this->socket_, result->ai_addr, (int)result->ai_addrlen);
	
	//if (iResult == SOCKET_ERROR) {
	//	cout << "bind failed with error: " << WSAGetLastError() << endl;
	//	freeaddrinfo(result);
	//	close();
	//	return false;
	//}

	//freeaddrinfo(result);

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

	sockaddr_in client;
	int clientSize = sizeof(client);

	//	Tạo một socket tạm để đồng ý kết nối
	SOCKET clientSocket = INVALID_SOCKET;

	// Accept a client socket
	clientSocket = accept(socket_, (sockaddr*)& client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		close();
		return false;
	}

	char host[NI_MAXHOST];		//client's remote name
	char service[NI_MAXSERV];	//service (i.e. port) client is connecting on
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	// lấy tên máy kết nối, in ra
	if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on " << service << endl;
	}
	else
	{
		// nếu không lấy tên được thì in IP
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}


	//	Chấp nhận xong thì không cần server socket nữa
	close();

	//	Chuyển socket_ về clientSocket để thực hiện tao tác xử lý (nhận, truyền lệnh, phản hồi,...)
	socket_ = clientSocket;

	
	return true;
}

void SocketServer::scan()
{
	char buf[4097];
	int byteReceived{}, iSendResult{};

	// Receive until the peer shuts down the connection
	do {
		ZeroMemory(buf, 4097);	

		byteReceived = recv(socket_, buf, 4097 - 1, 0);
		if (byteReceived > 0) {
			printf("Bytes received: %d\n", byteReceived);

			// Echo the buffer back to the sender
			iSendResult = send(socket_, buf, byteReceived, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				close();
				return;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (byteReceived == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			close();
			return;
		}

	} while (byteReceived > 0);
}
