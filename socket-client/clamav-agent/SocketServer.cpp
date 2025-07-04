#include "SocketServer.h"
#include "General.h"

string SocketServer::getResponseMessage(SOCKET& s)
{
	char responseMessage[4097];
	int iResult = recv(s, responseMessage, sizeof(responseMessage) - 1, 0);

	if (iResult > 0) {
		responseMessage[iResult] = '\0';
		return string(responseMessage); //tra ve gia tri de xu ly may cai khac
	}

	return "";
}

void SocketServer::sendCommandMessage(SOCKET& s, const char* msg)
{
	int iResult = send(s, msg, (int)strlen(msg), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
	}
}

//	Đóng socket
void SocketServer::close() {
	closesocket(listenSocket_);
	closesocket(clientSocket_);
}


bool SocketServer::clamavInput()
{
	cout << "Enter ClamAV folder: ";
	getline(cin, clamscanSource);

	// process string (erase redundant space " and split words)
	vector<string> temp = processCommandString(clamscanSource);

	if (temp.size() != 1)
	{
		cerr << "Invalid argument\n";
		return false;
	}

	clamscanSource = temp[0] + "\\clamscan.exe"; // add clamscan
	ifstream fin;
	fin.open(clamscanSource);
	if (!fin.is_open())
	{
		cerr << "Cannot find clamscan.exe\n";
		return false;
	}
	fin.close();

	cout << "===ClamAV Agent===\n";
	cout << "Waiting for file...\n";

	return true;
}

//	Init socket_ and bind the IP, port
bool SocketServer::clamavBind() {
	//	Lấy địa chỉ IP (IPV4) của máy host server (cái client đã nhập khi kết nối)
	//để làm IP cho server


	//init listenSocket_
	listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//argument cuối có thể là 0: OS sẽ SOCK_STREAM -> TCP
	
	if (listenSocket_ == INVALID_SOCKET) {
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
	iResult = bind(this->listenSocket_, (sockaddr*)& hints, sizeof(hints));
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << endl;
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
	if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

bool SocketServer::clamavAccept()
{
	//	Nếu có yêu cầu kết nối thì đồng ý kết nối

	sockaddr_in client;
	int clientSize = sizeof(client);

	// Accept a client socket
	clientSocket_ = accept(listenSocket_, (sockaddr*)& client, &clientSize);
	if (clientSocket_ == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		close();
		return false;
	}

	char host[NI_MAXHOST];		//client's remote name
	char service[NI_MAXSERV];	//service (i.e. port) client is connecting on
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	//===== Phần này có thể không cần
	//
	//// lấy tên máy kết nối, in ra
	//if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	//{
	//	cout << host << " connected on " << service << endl;
	//}
	//else
	//{
	//	// nếu không lấy tên được thì in IP
	//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	//	cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	//}===============

	//	Chấp nhận xong thì không cần dùng server socket nữa
	// Nhưng không closesocket vì vẫn còn giữ cái đấy để đợi đợt quét tiếp theo


	//  Trả lại dòng kết nối agent cho client
	string msg = "Scanning: ";
	sendCommandMessage(clientSocket_, msg.c_str());

	return true;
}

void SocketServer::scan()
{
	if (getResponseMessage(clientSocket_) == "I")
	{
		// Receive filename filesize and send response message
		string fileName = getResponseMessage(clientSocket_);
		string msg = "\"" + fileName + "\"";
		sendCommandMessage(clientSocket_, msg.c_str());

		string fileSize = getResponseMessage(clientSocket_);
		msg = " (" + fileSize + " Bytes)\n";
		sendCommandMessage(clientSocket_, msg.c_str());


		//https://en.cppreference.com/w/cpp/string/basic_string/stoul
		// Dùng 2 biến này để biết khi nào ghi lại hết nội dung file
		unsigned long long fileSizeNum = stoull(fileSize);
		unsigned long long totalBytesReceived = 0;

		//https://stackoverflow.com/questions/19017651/how-to-send-files-in-chunk-using-socket-c-c

		char buffer[CHUNK_SIZE];	//biến để nhận các chunk của file, sau đó dùng để ghi lại nội dung file
		int bytesReceived{};

		// Kiểm tra thư mục tmp có tồn tại hay không
		// Cần thư mục tmp để chưa file tạm, quét xong thì xóa bỏ (tránh xóa nhầm file code nếu trùng tên)
		//https://www.geeksforgeeks.org/cpp/how-to-check-a-file-or-directory-exists-in-cpp/
		const char* dir = "clamavtmp";

		struct stat sb;

		// Calls the function with path as argument
		// If the file/directory exists at the path returns 0
		// Nếu thư mục tmp không tồn tại thì tạo thư mục tmp
		if (stat(dir, &sb) != 0)
		{
			//cout << "The Path is invalid!\n";
			string mkdirCommand = "cmd /C \"mkdir " + string(dir) + "\"";

			system(mkdirCommand.c_str());
		}

		// Đổi tên file sang tmp\fileName
		fileName = string(dir) + "\\" + fileName;

		ofstream fout;
		fout.open(fileName, ios::binary | ios::trunc);	// trunc để mở cái file là xóa hết nội dung ở trong


		std::cout << "Receiving file...\n";
		// bytesReceived == 0: shutdown
		//				< 0: fail
		// Recv until reach fileSize
		while (totalBytesReceived < fileSizeNum)
		{
			// Không cần chừa chỗ cho \0 vì dùng write (ghi lại đúng số byte đã nhận)
			bytesReceived = recv(clientSocket_, buffer, CHUNK_SIZE, 0);

			if (bytesReceived == 0)
			{
				printf("Connection closing...\n");
				break;
			}
			else if (bytesReceived < 0) { // < 0: fail
				printf("recv failed: %d\n", WSAGetLastError());
				fout.close();

				return;
			}

			// > 0: receive success
			//printf("Bytes received: %d\n", bytesReceived);

			fout.write(buffer, bytesReceived);

			totalBytesReceived += bytesReceived;
		} 	
	
		fout.close();

		std::cout << "Received: " << totalBytesReceived << "/" << fileSizeNum << " bytes\n";


		// RUN clamscan <file>
		//https://linux.die.net/man/1/clamscan#:~:text=Return%20Codes,Some%20error(s)%20occured.
		// clamscan return value:
		// 0 : No virus found. 1 : Virus(es) found. 2 : Some error(s) occured.

		std::cout << "Running clamscan\n";
		string clamscanCommand = "cmd /C \"\"" + clamscanSource + "\" \"" + fileName + "\"\"";
	
		msg = "Scanning result: ";
		sendCommandMessage(clientSocket_, msg.c_str());

		int scanResult = system(clamscanCommand.c_str());
		sendCommandMessage(clientSocket_, to_string(scanResult).c_str());

	
		//tái sử dụng biến, nhưng dùng nó để xóa cái file đi (sau khi quét xong)
		clamscanCommand = string("cmd /C \"del ") + "\"" + fileName + "\"\"";
		system(clamscanCommand.c_str());

		std::cout << "\nScanning done!\n\n";
		std::cout << "Waiting for file...\n";
	}
	else // Ascii ("A")
	{
		// Receive filename filesize and send response message
		string fileName = getResponseMessage(clientSocket_);
		string msg = "\"" + fileName + "\"";
		sendCommandMessage(clientSocket_, msg.c_str());


		//https://stackoverflow.com/questions/19017651/how-to-send-files-in-chunk-using-socket-c-c

		char buffer[CHUNK_SIZE];	//biến để nhận các chunk của file, sau đó dùng để ghi lại nội dung file
		int bytesReceived{};

		// Kiểm tra thư mục tmp có tồn tại hay không
		// Cần thư mục tmp để chưa file tạm, quét xong thì xóa bỏ (tránh xóa nhầm file code nếu trùng tên)
		//https://www.geeksforgeeks.org/cpp/how-to-check-a-file-or-directory-exists-in-cpp/
		const char* dir = "clamavtmp";

		struct stat sb;

		// Calls the function with path as argument
		// If the file/directory exists at the path returns 0
		// Nếu thư mục tmp không tồn tại thì tạo thư mục tmp
		if (stat(dir, &sb) != 0)
		{
			//cout << "The Path is invalid!\n";
			string mkdirCommand = "cmd /C \"mkdir " + string(dir) + "\"";

			system(mkdirCommand.c_str());
		}

		// Đổi tên file sang tmp\fileName
		fileName = string(dir) + "\\" + fileName;

		ofstream fout;
		fout.open(fileName, ios::trunc);	// trunc để mở cái file là xóa hết nội dung ở trong


		std::cout << "Receiving file...\n";
		// bytesReceived == 0: shutdown
		//				< 0: fail
		// Recv until reach fileSize
		while ((bytesReceived = recv(clientSocket_, buffer, CHUNK_SIZE - 1, 0)) > 0)
		{
			// Không cần chừa chỗ cho \0 vì dùng write (ghi lại đúng số byte đã nhận)
			buffer[bytesReceived] = '\0';

			if (bytesReceived == 0)
			{
				printf("Connection closing...\n");
				break;
			}
			else if (bytesReceived < 0) { // < 0: fail
				printf("recv failed: %d\n", WSAGetLastError());
				fout.close();

				return;
			}

			fout << buffer;
		}

		fout.close();

		std::cout << "Received: " << " bytes\n";


		// RUN clamscan <file>
		//https://linux.die.net/man/1/clamscan#:~:text=Return%20Codes,Some%20error(s)%20occured.
		// clamscan return value:
		// 0 : No virus found. 1 : Virus(es) found. 2 : Some error(s) occured.

		std::cout << "Running clamscan\n";
		string clamscanCommand = "cmd /C \"\"" + clamscanSource + "\" \"" + fileName + "\"\"";

		msg = "Scanning result: ";
		sendCommandMessage(clientSocket_, msg.c_str());

		int scanResult = system(clamscanCommand.c_str());
		sendCommandMessage(clientSocket_, to_string(scanResult).c_str());


		//tái sử dụng biến, nhưng dùng nó để xóa cái file đi (sau khi quét xong)
		clamscanCommand = string("cmd /C \"del ") + "\"" + fileName + "\"\"";
		system(clamscanCommand.c_str());

		std::cout << "\nScanning done!\n\n";
		std::cout << "Waiting for file...\n";
	}
}

//void SocketServer::scanASCII()
//{
//	// Receive filename filesize and send response message
//	string fileName = getResponseMessage(clientSocket_);
//	string msg = "\"" + fileName + "\"";
//	sendCommandMessage(clientSocket_, msg.c_str());
//
//
//	//https://stackoverflow.com/questions/19017651/how-to-send-files-in-chunk-using-socket-c-c
//
//	char buffer[CHUNK_SIZE];	//biến để nhận các chunk của file, sau đó dùng để ghi lại nội dung file
//	int bytesReceived{};
//
//	// Kiểm tra thư mục tmp có tồn tại hay không
//	// Cần thư mục tmp để chưa file tạm, quét xong thì xóa bỏ (tránh xóa nhầm file code nếu trùng tên)
//	//https://www.geeksforgeeks.org/cpp/how-to-check-a-file-or-directory-exists-in-cpp/
//	const char* dir = "clamavtmp";
//
//	struct stat sb;
//
//	// Calls the function with path as argument
//	// If the file/directory exists at the path returns 0
//	// Nếu thư mục tmp không tồn tại thì tạo thư mục tmp
//	if (stat(dir, &sb) != 0)
//	{
//		//cout << "The Path is invalid!\n";
//		string mkdirCommand = "cmd /C \"mkdir " + string(dir) + "\"";
//
//		system(mkdirCommand.c_str());
//	}
//
//	// Đổi tên file sang tmp\fileName
//	fileName = string(dir) + "\\" + fileName;
//
//	ofstream fout;
//	fout.open(fileName, ios::trunc);	// trunc để mở cái file là xóa hết nội dung ở trong
//
//
//	std::cout << "Receiving file...\n";
//	// bytesReceived == 0: shutdown
//	//				< 0: fail
//	// Recv until reach fileSize
//	while ((bytesReceived = recv(clientSocket_, buffer, CHUNK_SIZE - 1, 0)) > 0)
//	{
//		// Không cần chừa chỗ cho \0 vì dùng write (ghi lại đúng số byte đã nhận)
//		buffer[bytesReceived] = '\0';
//
//		if (bytesReceived == 0)
//		{
//			printf("Connection closing...\n");
//			break;
//		}
//		else if (bytesReceived < 0) { // < 0: fail
//			printf("recv failed: %d\n", WSAGetLastError());
//			fout.close();
//
//			return;
//		}
//
//		fout << buffer;
//	}
//
//	fout.close();
//
//	std::cout << "Received: " << " bytes\n";
//
//
//	// RUN clamscan <file>
//	//https://linux.die.net/man/1/clamscan#:~:text=Return%20Codes,Some%20error(s)%20occured.
//	// clamscan return value:
//	// 0 : No virus found. 1 : Virus(es) found. 2 : Some error(s) occured.
//
//	std::cout << "Running clamscan\n";
//	string clamscanCommand = "cmd /C \"\"" + clamscanSource + "\" \"" + fileName + "\"\"";
//
//	msg = "Scanning result: ";
//	sendCommandMessage(clientSocket_, msg.c_str());
//
//	int scanResult = system(clamscanCommand.c_str());
//	sendCommandMessage(clientSocket_, to_string(scanResult).c_str());
//
//
//	//tái sử dụng biến, nhưng dùng nó để xóa cái file đi (sau khi quét xong)
//	clamscanCommand = string("cmd /C \"del ") + "\"" + fileName + "\"\"";
//	system(clamscanCommand.c_str());
//
//	std::cout << "\nScanning done!\n\n";
//	std::cout << "Waiting for file...\n";
//}


