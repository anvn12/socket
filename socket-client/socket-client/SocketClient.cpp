#include "SocketClient.h"
#include "General.h"

void SocketClient::close() {
	if (socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}

void SocketClient::inputCommand() {
	string temp;
	getline(cin, temp);

	// process string (erase redundant space, split words)
	command = processCommandString(temp);
}


//bool SocketClient::connectToServer(addrinfo* result) {
//	int iResult{};		// for checking
//	// attempt to connect to the server
//	for (int num_attempt = 0; num_attempt < 10; ++num_attempt) {
//		this->socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
//		if (this->socket_ == INVALID_SOCKET) { // fail to create socket
//			std::cerr << "Socket failed: " << WSAGetLastError() << "\n";
//			freeaddrinfo(result);
//			WSACleanup();
//			return 1;
//		}
//
//		// connect to the server
//		iResult = connect(this->socket_, result->ai_addr, (int)result->ai_addrlen);
//		if (iResult == 0) { // success
//			break;
//		}
//
//		// fail to connect to the server
//		std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
//		closesocket(this->socket_);
//		this->socket_ = INVALID_SOCKET;
//	}
//
//	if (socket_ == INVALID_SOCKET) { // fail to connect
//		std::cerr << "Unable to connect to server!" << std::endl;
//		return false;
//	}
//
//	return true;
//}

bool SocketClient::processCommand()
{
	//	Muốn hiện câu "invalid command" => return false
	// 
	//	Mỗi hàm sẽ gửi 1 số command cho ftp server, server sẽ nhận command
	//làm công việc tương ứng, sau đó phản hồi (response) lại.


	//	Connect to the FTP server
	if (command[0] == "open") // open <IP>
	{
		//	Nếu đã kết nối thì không kết nối lại
		if (isConnected == true)
		{
			cout << "Already connected to " << serverIP << ", disconnect first.\n";
			return true;
		}

		//	Nếu số lượng argument khác 2 (câu lệnh đúng theo kiểu: "open <IP>")
		if (command.size() != 2)
		{
			//cout << "Invalid command.\n";
			return false;
		}

		//	open <IP>: command[1] chỉnh là <IP>
		//	Kiểm tra IP có hợp lệ không
		serverIP = command[1];
		if (isValidIP(serverIP) == false)
		{
			return true;
		}

		//========================================================================
		
		////hints: give the supported type (IPV4 TCP)
		//addrinfo* result = NULL, hints;
		//memset(&hints, 0, sizeof(hints));	//clear all data of hints
		////IPV4 and TCP
		//hints.ai_family = AF_INET;			//ipv4
		//hints.ai_socktype = SOCK_STREAM;	//tcp protocol
		//hints.ai_protocol = IPPROTO_TCP;	//tcp protocol

		//// get the server address and port, put to result
		//int iResult = getaddrinfo(serverIP.c_str(), port, &hints, &result);
		//if (iResult != 0) { //fail to get address
		//	std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		//	isQuit = true;
		//	return true;
		//}

		//// attemp to connect
		//if (connectToServer(result) == false) {
		//	freeaddrinfo(result);
		//	isQuit = true;
		//	return true;
		//}

		//// connected
		//isConnected = true;
		//std::cout << "Connected to " << serverIP << endl;
		//freeaddrinfo(result);

		//========================================================================


		SOCKET newSocket = createConnection(serverIP, port, true);  // true for retry
		if (newSocket == INVALID_SOCKET) {
			isQuit = true;
			return true;
		}

		socket_ = newSocket;

		//	Kết nối thành công
		isConnected = true;
		cout << "Connected to " << serverIP << endl;

		//	Nhận tin nhắn phản hồi cho việc kết nối thành công
		cout << getResponseMessage();

		//	Định dạng việc gửi command sang UTF8
		//nhằm đảm bảo các command không bị sai định dạng
		sendCommandMessage("OPTS UTF8 ON\r\n");
		cout << getResponseMessage();		//tin nhắn phản hồi cho UTF8


		//	Yêu cầu nhập username
		cout << "Username: ";
		getline(cin, username);


		//	Câu command để gửi username cho FTP server
		string tempMsg = "USER " + username + "\r\n";

		//	Gửi command và nhận phản hồi
		sendCommandMessage(tempMsg.c_str());
		cout << getResponseMessage();

		
		//	Nhập password
		cout << "Password: ";
		getline(cin, password);

		//	Câu command để gửi password cho FTP server
		tempMsg = "PASS " + password + "\r\n";

		//	Gửi command và nhận phản hồi
		sendCommandMessage(tempMsg.c_str());
		cout << getResponseMessage();

		return true;
	}
	//	Disconnect to the FTP server
	else if (command[0] == "close")		//close
	{
		if (isConnected == false)	// not connected
		{
			cout << "Not connected.\n";
			return true;
		}

		if (command.size() != 1)
		{
			//cout << "Invalid command.\n";
			return false;
		}

		sendCommandMessage("QUIT\r\n");
		cout << getResponseMessage();

		// shutdown the send half of the connection since no more data will be sent
		int iResult = shutdown(socket_, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			isQuit = true;
			return true;
		}

		isConnected = false;
		close();
		
		return true;
	}
	//exit the FTP client
	else if (command[0] == "quit" || command[0] == "bye")	// quit or bye
	{
		if (command.size() != 1)
		{
			return false;
		}

		// if not connect
		if (isConnected == false)
		{
			isQuit = true;
			return true;
		}


		sendCommandMessage("QUIT\r\n");
		cout << getResponseMessage();

		// shutdown the send half of the connection since no more data will be sent
		int iResult = shutdown(socket_, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			isQuit = true;
			return false;
		}

		isConnected = false;
		isQuit = true;
		close();

		return true;
	}
	//Show the current directory on the server
	else if (command[0] == "pwd")	
	{
		// if not connect
		if (isConnected == false)
		{
			cout << "Not connected.\n";
			return true;
		}

		if (command.size() != 1)
		{
			return false;
		}


		sendCommandMessage("XPWD\r\n");
		cout << getResponseMessage();

		return true;
	}
	//change directory
	else if (command[0] == "cd") 
	{
		if (!isConnected) { 
			cout << "Not connected.\n"; 
			return true; 
		}
		string folder = getArgOrPrompt(command, 1, "Remote directory: ");
		if (folder.empty()) {
			cout << "No directory was entered.\n";
			return true;
		}
		sendCommandMessage(("CWD " + folder + "\r\n").c_str());
		cout << getResponseMessage();
		return true;
	}
	//Create folder
	else if (command[0] == "mkdir") 
	{
		if (!isConnected) { 
			cout << "Not connected.\n"; 
			return true; 
		}
		string folder = getArgOrPrompt(command, 1, "Folder name: ");
		if (folder.empty()) {
			cout << "No folder name was entered.\n";
			return true;
		}
		sendCommandMessage(("XMKD " + folder + "\r\n").c_str());
		cout << getResponseMessage();
		return true;
	}
	//remove folder
	else if (command[0] == "rmdir") {
		if (!isConnected) { 
			cout << "Not connected.\n"; 
			return true; 
		}
		string folder = getArgOrPrompt(command, 1, "Folder name: ");
		if (folder.empty()) {
			cout << "No folder name was entered.\n";
			return true;
		}
		sendCommandMessage(("XRMD " + folder + "\r\n").c_str());
		cout << getResponseMessage();
		return true;
	}
	//delete a file
	else if (command[0] == "delete") {
		if (!isConnected) { 
			cout << "Not connected.\n"; 
			return true; 
		}
		string file = getArgOrPrompt(command, 1, "Enter file name: ");
		if (file.empty()) {
			cout << "No file name was entered.\n";
			return true;
		}
		sendCommandMessage(("DELE " + file + "\r\n").c_str());
		cout << getResponseMessage();
		return true;
	}
	//rename kiểu interactive, là nhập rename xong nhập từng cái name
	//chứ dùng kiểu rename filename newname thì không được, t làm 1 lồn lỗi
	else if (command[0] == "rename") {
		if (!isConnected) {
			cout << "Not connected.\n";
			return true;
		}

		if (command.size() != 1) {
			return false;
		}

		string originalName, newName;
		cout << "From name: ";
		getline(cin, originalName);
		if (originalName.empty()) {
			cout << "No source name was entered.\n";
			return true;
		}

		cout << "To name: ";
		getline(cin, newName);
		if (newName.empty()) {
			cout << "No new name was entered.\n";
			return true;
		}


		sendCommandMessage(("RNFR " + originalName + "\r\n").c_str());
		cout << getResponseMessage(); 
		sendCommandMessage(("RNTO " + newName + "\r\n").c_str());
		cout << getResponseMessage();  

		return true;
	}

	else if (command[0] == "status")
	{
		if (command.size() != 1) {
			return false;
		}

		if (isConnected) {
			cout << "Connected to " << serverIP << ".\n";
			cout << "Type: ascii; Verbose: On ; Bell: Off ; Prompting: On ; Globbing: On\n"; //cái ascii có thể thay đổi lúc làm xong ascii/binary
			cout << "Debugging: Off ; Hash mark printing: Off .\n";
		}
		else {
			cout << "Not connected.\n";
		}

		return true;
	}

	else if (command[0] == "help" || command[0] == "?")
	{
		if (command.size() > 2) {
			return false;
		}

		else {
			// Hiển thị toàn bộ lệnh
			cout << "Commands may be abbreviated. Commands are:\n\n";
			cout << "               delete         literal        prompt         send\n";
			cout << "?              debug          ls             put            status\n";
			cout << "append         dir            mdelete        pwd            trace\n";
			cout << "ascii          disconnect     mdir           quit           type\n";
			cout << "bell           get            mget           quote          user\n";
			cout << "binary         glob           mkdir          recv           verbose\n";
			cout << "bye            hash           mls            remotehelp     \n";
			cout << "cd             help           mput           rename         \n";
			cout << "close          lcd            open           rmdir          \n";
		}

		return true;
	}
	//co 2 loai conenction la control conenction voi data conenction
	//port 21 la de control connection
	//data connection la cho cac lenh lien quan den thay doi data,.. nhu LIST, RETR, STOR nen can 1 cai port khac, can phai lay cai port voi ip khac 
	//1. sendCommandMessage("PASV\r\n");
	//2. parse the response to get IP and port
	//3. open a new socket connection to that port
	//4. sendCommandMessage("LIST\r\n");
	//5. read from the data socket
	//6. close the data socket
	//7. getResponseMessage(); // final server reply
	else if (command[0] == "ls") {
		// if not connect
		if (isConnected == false)
		{
			cout << "Not connected.\n";
			return true;
		}
		if (command.size() != 1)
		{
			return false;
		}

		////1. gui PASV de vao passive mode
		//sendCommandMessage("PASV\r\n");
		//string response = getResponseMessage();
		//cout << response;
		//string dataIP, dataPort;

		//try {
		//	tie(dataIP, dataPort) = parsePASVResponse(response);
		//}
		//catch (const exception& ex) {
		//	cerr << ex.what() << "\n";
		//	return false;
		//}

		//SOCKET dataSocket = createDataConnection(dataIP, dataPort);
		//if (dataSocket == INVALID_SOCKET) {
		//	cerr << "Failed to create data connection \n";
		//	return true;
		//}

		string localIP;
		int localPort;
		SOCKET listenSocket = createListeningSocket(localIP, localPort);
		if (listenSocket == INVALID_SOCKET) {
			cerr << "Failed to create listening socket for PORT mode";
			return true;
		}

		string portCommand = formatPORTCommand(localIP, localPort);
		sendCommandMessage(portCommand.c_str());
		cout << getResponseMessage();

		//gui cai lenh NLST
		sendCommandMessage("NLST\r\n");

		//in cai 150 ra truoc
		cout << getResponseMessage();
		//accept incoming data connection
		SOCKET dataSocket = accept(listenSocket, nullptr, nullptr);
		closesocket(listenSocket);

		if (dataSocket == INVALID_SOCKET) {
			cerr << "Failed to accept data connection";
			return true;
		}
		
		//doan nay tro xuong k khac gi pasv mode
		//bat dau doc directory tu data socket
		char buffer[4096];
		string directoryListing;
		int bytesReceived;

		//int totalBytes = 0;
		//auto start = chrono::high_resolution_clock::now(); 

		while ((bytesReceived = recv(dataSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
			buffer[bytesReceived] = '\0';
			directoryListing += buffer;
			//totalBytes += bytesReceived;
		}

		//auto end = chrono::high_resolution_clock::now();  

		//chrono::duration<double> duration = end - start;  //seconds

		//double seconds = duration.count();
		//double kilobytes = totalBytes / 1024.0;
		//double speed = (seconds > 0) ? (kilobytes / seconds) : 0;

		//dong cai data socket
		closesocket(dataSocket);
		//list cai directory ra
		if (!directoryListing.empty()) {
			cout << directoryListing << "\n";
		}
		//cout << "ftp: " << totalBytes << " bytes received in " << seconds << " seconds " << speed << " Kbytes/sec.\n";
		cout << getResponseMessage();

		return true;
	}

	//else if (command[0] == "get" || command[0] == "recv") {}
	//else if (command[0] == "mget") {}
	//else if (command[0] == "prompt") {}
	//else if (command[0] == "ascii / binary") {}
	//else if (command[0] == "status") {}
	//else if (command[0] == "passive") {}
	//else if (command[0] == "help" || command[0] == "?") {}

	

	// clamscan agent.....................

	else if (command[0] == "put") 
	{
		// put <filePath>
		// 
		// if not connect
		if (isConnected == false)
		{
			cout << "Not connected.\n";
			return true;
		}
		if (command.size() != 2)
		{
			return false;
		}

		//https://cplusplus.com/doc/tutorial/files/
		//  MỞ file
		string filePath = command[1];  // "D:\Folder A\fileA.txt"
		ifstream fin;
		fin.open(filePath, ios::binary | ios::ate);	//file nhị phân để đọc nội dung -> chuyển nội dung đi
											// mở từ cuối file để lấy kích thước nhanh hơn
		if (!fin.is_open())
		{
			cout << "File not found or cannot open file.\n";
			return true;
		}
		
		//  Lấy kích thươc tệp (byte)
		uint64_t fileSize = fin.tellg();
	
		//  Dời tới đầu file để đọc
		fin.seekg(0, ios::beg);


		//filePath: D:\Folder A\fileA.txt
		//	Get file name
		string fileName;
		for (int i = filePath.size() - 1; i >= 0; i--)
		{
			if (filePath[i] == '\\')
			{
				break;
			}
			fileName.push_back(filePath[i]);
		}
		reverse(fileName.begin(), fileName.end());

		//	Truyền file qua cho ClamAV Agent để quét
		//  Tạo socket để truyền, 
		//ClamAV Agent đặt cùng với FTP server nên cùng IP, port tự đặt
		SOCKET clamavSocket = createConnection(serverIP, clamavPort, true);  // true for retry
		if (clamavSocket == INVALID_SOCKET) {
			cerr << "Cannot init socket\n";
			isQuit = true;
			return true;
		}


		//  first message "Scanning: " => kết nối được agent
		cout << getResponseMessage(clamavSocket);

		//https://youtu.be/NHrk33uCzL8?si=F2rQr1mvSjYWPuHQ
		// 
		//  Chuyển tên file
		sendCommandMessage(clamavSocket, fileName.c_str());
		cout << getResponseMessage(clamavSocket);

		//  Chuyển kích thước 
		sendCommandMessage(clamavSocket, to_string(fileSize).c_str());
		cout << getResponseMessage(clamavSocket);

		//  Chuyển nội dung file theo từng chunk, tránh chuyển đi quá nhiều trong 1 lần
		char buffer[CHUNK_SIZE];

		while (!fin.eof()) 
		{
			// Đọc file binary theo từng chunk 
			fin.read(buffer, CHUNK_SIZE);

			int bytesRead = fin.gcount();		// số kí tự đọc được

			// gửi chunk vừa đọc qua agent
			send(clamavSocket, buffer, bytesRead, 0);
		}

		//  Bên agent: tạo file mới, đọc lại nội dung rồi bỏ vào file

		cout << getResponseMessage(clamavSocket);

		int iResult = stoi(getResponseMessage(clamavSocket));

		// OK
		if (iResult == 0)
		{
			cout << "OK\n";

			// send file to ftp server

			//ftp > put "D:\a a.txt
			//	200 PORT command successful.
			// 
			// command 222222222222222222222222222
			//	150 Starting data transfer.
			//	226 Operation successful
			//	ftp : 5 bytes sent in 0.00Seconds 5.00Kbytes / sec.


			// Lấy ip, port server (cái mà client đang kết nối vô)
			string localIP;
			int localPort;
			SOCKET listenSocket = createListeningSocket(localIP, localPort);
			if (listenSocket == INVALID_SOCKET) {
				cerr << "Failed to create listening socket for PORT mode";
				return true;
			}

			string portCommand = formatPORTCommand(localIP, localPort);
			sendCommandMessage(portCommand.c_str());
			cout << getResponseMessage();

			//gui cai lenh STOR
			string msg = "STOR " + fileName + "\r\n";
			sendCommandMessage(msg.c_str());

			//in cai 150 ra truoc (bắt đầu gửi cái nội dung file qua)
			cout << getResponseMessage();

			//accept incoming data connection
			SOCKET dataSocket = accept(listenSocket, nullptr, nullptr);
			closesocket(listenSocket);
			if (dataSocket == INVALID_SOCKET) {
				cerr << "Failed to accept data connection";
				return true;
			}

			//https://stackoverflow.com/questions/5343173/returning-to-beginning-of-file-after-getline
			//  Dời tới đầu file để đọc
			fin.clear();		// do lúc gửi cho agent đã chạm tới eof -> clear
			fin.seekg(0, ios::beg);

			while (!fin.eof())
			{
				// Đọc file binary theo từng chunk 
				fin.read(buffer, CHUNK_SIZE);

				int bytesRead = fin.gcount();		// số kí tự đọc được

				// gửi chunk vừa đọc qua server
				send(dataSocket, buffer, bytesRead, 0);
			}

			//dong cai data socket
			closesocket(dataSocket);

			cout << getResponseMessage();
		}
		else if (iResult == 1)
		{
			cout << "Warning\n";
		}
		else
		{
			cout << "ERROR\n";
		}










		fin.close();
		closesocket(clamavSocket);

		//char responseMessage[4097];
		//int iResult = recv(clamavSocket, responseMessage, sizeof(responseMessage) - 1, 0);

		////  Vì trả về không có \0 nên phải thêm vào
		//if (iResult > 0) {
		//	responseMessage[iResult] = '\0';
		//}
		//cout << responseMessage;







		// OK -> truyen cho FTP server

		return true;
	}
	//else if (command[0] == "mput") {}
	
	else
	{
		return false;
	}
}

string SocketClient::getResponseMessage()
{
	char responseMessage[4097];
	int iResult = recv(socket_, responseMessage, sizeof(responseMessage) - 1, 0);

	if (iResult > 0) {
		responseMessage[iResult] = '\0';
		//cout << responseMessage;
		return string(responseMessage); //tra ve gia tri de xu ly may cai khac
	}

	return "";
}

string SocketClient::getResponseMessage(SOCKET &s)
{
	char responseMessage[4097];
	int iResult = recv(s, responseMessage, sizeof(responseMessage) - 1, 0);

	if (iResult > 0) {
		responseMessage[iResult] = '\0';
		//cout << responseMessage;
		return string(responseMessage); //tra ve gia tri de xu ly may cai khac
	}

	return "";
}

void SocketClient::sendCommandMessage(const char* msg)
{
	int iResult = send(socket_, msg, (int)strlen(msg), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		isQuit = true;
	}
}

void SocketClient::sendCommandMessage(SOCKET& s, const char* msg)
{
	int iResult = send(s, msg, (int)strlen(msg), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		isQuit = true;
	}
}

SOCKET SocketClient::createConnection(const string& ip, const string& port, bool withRetry) {
	SOCKET newSocket = INVALID_SOCKET;
	addrinfo* result = NULL;
	addrinfo hints;

	//	hints: dùng cho định dạng kiểu TCP, IPV4 cho socket
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;        //ipv4
	hints.ai_socktype = SOCK_STREAM;  //tcp
	hints.ai_protocol = IPPROTO_TCP;  //tcp

	//lay dia chi
	int iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		cerr << "getaddrinfo failed: " << iResult << endl;
		//closesocket(newSocket); k can close do newSocket = invalid
		return INVALID_SOCKET;
	}

	//	Thử lại nếu kết nối không thành công
	//withRetry == true: thử lại 10 lần
	//withRetry == false: không thử lại
	int maxAttempts = withRetry ? 10 : 1;
	for (int attempt = 0; attempt < maxAttempts; ++attempt) {
		newSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (newSocket == INVALID_SOCKET) {
			cerr << "Socket creation failed: " << WSAGetLastError() << endl;
			if (attempt == maxAttempts - 1) {  //lan thu cuoi thi free data
				freeaddrinfo(result);
				return INVALID_SOCKET;
			}
			continue;
		}

		//connect to server
		iResult = connect(newSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == 0) {  //thnah cong
			break;
		}

		//failed
		if (!withRetry || attempt == maxAttempts - 1) {
			cerr << "Connection failed: " << WSAGetLastError() << endl;
		}
		closesocket(newSocket);
		newSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);
	return newSocket;
}

//wrap function, tai t thay m dung nhu kieu kia, con t dung kieu khac =))) 
SOCKET SocketClient::createDataConnection(const string& dataIP, const string& dataPort) {
	return createConnection(dataIP, dataPort, false);  //do la data connection nen k dung retry
}

SOCKET SocketClient::createListeningSocket(string& localIP, int& localPort) {
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		cerr << "FAiled to create listening socket: " << WSAGetLastError() << "\n";
		return INVALID_SOCKET;
	}

	//lay cai local IP cua control connection
	sockaddr_in localAddr;
	int addrLen = sizeof(localAddr);

	//lay dia chi local cua control connection SOCKET
	if (getsockname(socket_, (sockaddr*)&localAddr, &addrLen) == SOCKET_ERROR) {
		cerr << "getsockname failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		return INVALID_SOCKET;
	}

	//doi no sang string
	char ipStr[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &(localAddr.sin_addr), ipStr, INET_ADDRSTRLEN) == nullptr) {
		cerr << "inet_ntop failed" << endl;
		closesocket(listenSocket);
		return INVALID_SOCKET;
	}
	localIP = string(ipStr);

	//bind cai ip cho giong voi control connection
	sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr = localAddr.sin_addr; //giong voi control connection
	bindAddr.sin_port = 0; //system tu chon

	if (bind(listenSocket, (sockaddr*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR) {
		cerr << "Bind failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		return INVALID_SOCKET;
	}

	//lay cai port system vua assign
	addrLen = sizeof(bindAddr);
	if (getsockname(listenSocket, (sockaddr*)&bindAddr, &addrLen) == SOCKET_ERROR) {
		cerr << "getsockname failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		return INVALID_SOCKET;
	}
	localPort = ntohs(bindAddr.sin_port);

	if (listen(listenSocket, 1) == SOCKET_ERROR) {
		cerr << "Listen failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		return INVALID_SOCKET;
	}

	return listenSocket;
}

//PORT h1,h2,h3,h4,p1,p2
string SocketClient::formatPORTCommand(const string& ip, int port) {
	string formatIP = ip;
	for (char& c : formatIP) {
		if (c == '.')
			c = ',';
	}

	//tinh so byte cua port
	int highByte = port / 256;
	int lowByte= port % 256;

	return "PORT " + formatIP + "," + to_string(highByte) + "," + to_string(lowByte) + "\r\n";
}

