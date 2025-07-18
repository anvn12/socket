﻿#include "SocketClient.h"
#include "General.h"

void SocketClient::close() {
	if (socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}

void SocketClient::inputCommand() {
	string temp;
	std::getline(cin, temp);

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
			std::cout << "Already connected to " << serverIP << ", disconnect first.\n";
			return true;
		}

		//	Nếu số lượng argument khác 2 (câu lệnh đúng theo kiểu: "open <IP>")
		if (command.size() != 2)
		{
			//std::cout << "Invalid command.\n";
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
		std::cout << "Connected to " << serverIP << endl;

		//	Nhận tin nhắn phản hồi cho việc kết nối thành công
		std::cout << getResponseMessage();

		//	Định dạng việc gửi command sang UTF8
		//nhằm đảm bảo các command không bị sai định dạng
		sendCommandMessage("OPTS UTF8 ON\r\n");
		std::cout << getResponseMessage();		//tin nhắn phản hồi cho UTF8


		//	Yêu cầu nhập username
		std::cout << "Username: ";
		std::getline(cin, username);


		//	Câu command để gửi username cho FTP server
		string tempMsg = "USER " + username + "\r\n";

		//	Gửi command và nhận phản hồi
		sendCommandMessage(tempMsg.c_str());
		std::cout << getResponseMessage();

		
		//	Nhập password
		std::cout << "Password: ";
		std::getline(cin, password);

		//	Câu command để gửi password cho FTP server
		tempMsg = "PASS " + password + "\r\n";

		//	Gửi command và nhận phản hồi
		sendCommandMessage(tempMsg.c_str());
		std::cout << getResponseMessage();

		return true;
	}
	//	Disconnect to the FTP server
	else if (command[0] == "close")		//close
	{
		if (isConnected == false)	// not connected
		{
			std::cout << "Not connected.\n";
			return true;
		}

		if (command.size() != 1)
		{
			//std::cout << "Invalid command.\n";
			return false;
		}

		sendCommandMessage("QUIT\r\n");
		std::cout << getResponseMessage();

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
		std::cout << getResponseMessage();

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
			std::cout << "Not connected.\n";
			return true;
		}

		if (command.size() != 1)
		{
			return false;
		}


		sendCommandMessage("XPWD\r\n");
		std::cout << getResponseMessage();

		return true;
	}
	//change directory
	else if (command[0] == "cd") 
	{
		if (!isConnected) { 
			std::cout << "Not connected.\n"; 
			return true; 
		}
		if (command.size() != 2)
		{
			return false;
		}

		string folder = command[1];

		/*string folder = getArgOrPrompt(command, 1, "Remote directory: ");
		if (folder.empty()) {
			std::cout << "No directory was entered.\n";
			return true;
		}*/
		sendCommandMessage(("CWD " + folder + "\r\n").c_str());
		std::cout << getResponseMessage();
		return true;
	}
	//Create folder
	else if (command[0] == "mkdir") 
	{
		if (!isConnected) { 
			std::cout << "Not connected.\n"; 
			return true; 
		}
		if (command.size() != 2)
		{
			return false;
		}
		string folder = command[1];

		/*string folder = getArgOrPrompt(command, 1, "Folder name: ");
		if (folder.empty()) {
			std::cout << "No folder name was entered.\n";
			return true;
		}*/
		sendCommandMessage(("XMKD " + folder + "\r\n").c_str());
		std::cout << getResponseMessage();
		return true;
	}
	//remove folder
	else if (command[0] == "rmdir") {
		if (!isConnected) { 
			std::cout << "Not connected.\n"; 
			return true; 
		}
		if (command.size() != 2)
		{
			return false;
		}
		string folder = command[1];
		/*string folder = getArgOrPrompt(command, 1, "Folder name: ");
		if (folder.empty()) {
			std::cout << "No folder name was entered.\n";
			return true;
		}*/
		sendCommandMessage(("XRMD " + folder + "\r\n").c_str());
		std::cout << getResponseMessage();
		return true;
	}
	//delete a file
	else if (command[0] == "delete") {
		if (!isConnected) { 
			std::cout << "Not connected.\n"; 
			return true; 
		}
		if (command.size() != 2)
		{
			return false;
		}
		string file = command[1];

		/*string file = getArgOrPrompt(command, 1, "Enter file name: ");
		if (file.empty()) {
			std::cout << "No file name was entered.\n";
			return true;
		}*/
		sendCommandMessage(("DELE " + file + "\r\n").c_str());
		std::cout << getResponseMessage();
		return true;
	}
	//rename a file
	else if (command[0] == "rename") {
		if (!isConnected) {
			std::cout << "Not connected.\n";
			return true;
		}

		if (command.size() != 3) {
			return false;
		}

		string originalName = command[1];
		string newName = command[2];

		/*string originalName, newName;
		std::cout << "From name: ";
		std::getline(cin, originalName);
		if (originalName.empty()) {
			std::cout << "No source name was entered.\n";
			return true;
		}

		std::cout << "To name: ";
		std::getline(cin, newName);
		if (newName.empty()) {
			std::cout << "No new name was entered.\n";
			return true;
		}*/


		sendCommandMessage(("RNFR " + originalName + "\r\n").c_str());
		std::cout << getResponseMessage(); 
		sendCommandMessage(("RNTO " + newName + "\r\n").c_str());
		std::cout << getResponseMessage();  

		return true;
	}

	else if (command[0] == "status")
	{
		if (command.size() != 1) {
			return false;
		}

		if (isConnected) {
			std::cout << "Connected to " << serverIP << ".\n";
		}
		else {
			std::cout << "Not connected.\n";
		}

		// ON
		// type: ascii (text) / binary (non-text)
		// prompting: yes no cho mget mput
		// mode: active (PORT) / passive (PASV)
		// 
		// verbose: hiện mấy cái response message
		// bell: kêu tiếng khi gửi command
		// globbing: cho phép *.txt là chọn tất cả file .txt
		// debugging: hiện mấy cái command gửi đi
		// hash...: in mấy dấu # trong lúc truyền nhận file
		if (type == 'A') std::cout << "Type: ASCII; ";
		else std::cout << "Type: Binary; ";

		if (passiveMode == true) std::cout << "Mode: Passive;\n";
		else std::cout << "Mode: Active;\n";
		
		if (promptMode == true) std::cout << "Prompting: On.\n";
		else std::cout << "Prompting: Off.\n";

		return true;
	}

	else if (command[0] == "ls") {
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
		// if not connect
		if (isConnected == false)
		{
			std::cout << "Not connected.\n";
			return true;
		}
		if (command.size() != 1)
		{
			return false;
		}

		string localIP;
		int localPort;
		SOCKET dataOrListen = establishDataConnection(localIP, localPort);
		if (dataOrListen == INVALID_SOCKET) {
			cerr << "Failed to create data connection\n";
			return false;
		}

		sendCommandMessage("NLST\r\n");
		cout << getResponseMessage();  // 150

		//result = condition ? value_if_true : value_if_false;
		//de kieu cu dai qua t sua lai ntn 
		SOCKET dataSocket = passiveMode ? dataOrListen : accept(dataOrListen, nullptr, nullptr);
		if (!passiveMode) {
			closesocket(dataOrListen);
		}
		if (dataSocket == INVALID_SOCKET) {
			cerr << "Failed to accept data connection\n";
			return false;
		}

		
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
			std::cout << directoryListing << "\n";
		}
		//std::cout << "ftp: " << totalBytes << " bytes received in " << seconds << " seconds " << speed << " Kbytes/sec.\n";
		std::cout << getResponseMessage();

		return true;
	}

	//else if (command[0] == "ascii / binary") {}
	else if (command[0] == "passive") {
		if (isConnected == false)
		{
			std::cout << "Not connected.\n";
			return true;
		}
		if (command.size() != 1)
		{
			return false;
		}

		passiveMode = !passiveMode;
		std::cout << "Passive mode " << (passiveMode ? "On" : "Off") << ".\n";

		return true;
	}

	// bật/tắt việc hỏi xác nhận từng file khi dùng mget/mput
	else if (command[0] == "prompt") {
		if (command.size() != 1) return false;
		promptMode = !promptMode;
		std::cout << "Interactive prompting " << (promptMode ? "On" : "Off") << ".\n";
		return true;
	}

	// tải 1 file từ FTP server về client
	else if (command[0] == "get" || command[0] == "recv") {
		if (!isConnected) { 
			std::cout << "Not connected.\n"; return true; 
		}

		if (command.size() != 2)
		{
			return false;
		}

		char prevType = type;

		do
		{
			std::cout << "Select transfer mode: [A] ASCII or [I] Binary: ";

			string inputMode;
			std::getline(cin, inputMode);
			if (inputMode == "A" || inputMode == "a" ||
				inputMode == "ascii" || inputMode == "ASCII")
			{
				// run ascii command
				type = 'A';
				sendCommandMessage("TYPE A\r\n");
				cout << getResponseMessage();

				break;
			}
			else if (inputMode == "I" || inputMode == "i" ||
				inputMode == "binary" || inputMode == "BINARY")
			{
				// run binary command
				type = 'I';
				sendCommandMessage("TYPE I\r\n");
				cout << getResponseMessage();
				break;
			}

			std::cout << "Invalid mode!\n";
		} while (true);


		string filename = command[1];

		//string filename = getArgOrPrompt(command, 1, "Remote file name: ");
		//if (filename.empty()) return true;
		if (type == 'A') {
			get1FileASCII(filename);
		}
		else {
			get1File(filename);
		}

		type = prevType;

		return true;
	}

	// tải nhiều file từ FTP server
	else if (command[0] == "mget") {
		if (!isConnected) { 
			std::cout << "Not connected.\n"; return true; 
		}
		if (command.size() < 2) return false;

		char prevType = type;

		for (size_t i = 1; i < command.size(); ++i) {
			if (promptMode) {
				std::cout << "Get \"" << command[i] << "\"? (Press 'y' to download): ";
				string res;
				std::getline(cin, res);
				if (res != "y" && res != "Y") continue;
			}


			do
			{
				std::cout << "Select transfer mode: [A] ASCII or [I] Binary: ";

				string inputMode;
				std::getline(cin, inputMode);
				if (inputMode == "A" || inputMode == "a" ||
					inputMode == "ascii" || inputMode == "ASCII")
				{
					// run ascii command
					type = 'A';
					sendCommandMessage("TYPE A\r\n");
					cout << getResponseMessage();

					break;
				}
				else if (inputMode == "I" || inputMode == "i" ||
					inputMode == "binary" || inputMode == "BINARY")
				{
					// run binary command
					type = 'I';
					sendCommandMessage("TYPE I\r\n");
					cout << getResponseMessage();
					break;
				}

				std::cout << "Invalid mode!\n";
			} while (true);


			if (type == 'A') {
				get1FileASCII(command[i]);
			}
			else {
				get1File(command[i]);
			}
			std::cout << "\n";
		}

		type = prevType;

		return true;
	}
	

	// clamscan agent.....................

	else if (command[0] == "put")
	{
		// put <filePath>
		// 
		// if not connect
		if (isConnected == false)
		{
			std::cout << "Not connected.\n";
			return true;
		}
		if (command.size() != 2)
		{
			return false;
		}

		char prevType = type;

		do
		{
			std::cout << "Select transfer mode: [A] ASCII or [I] Binary: ";

			string inputMode;
			std::getline(cin, inputMode);
			if (inputMode == "A" || inputMode == "a" ||
				inputMode == "ascii" || inputMode == "ASCII")
			{
				// run ascii command
				type = 'A';
				sendCommandMessage("TYPE A\r\n");
				cout << getResponseMessage();

				break;
			}
			else if (inputMode == "I" || inputMode == "i" ||
				inputMode == "binary" || inputMode == "BINARY")
			{
				// run binary command
				type = 'I';

				sendCommandMessage("TYPE I\r\n");
				cout << getResponseMessage();

				break;
			}

			std::cout << "Invalid mode!\n";
		} while (true);

		

		// Chuyển file đi
		std::cout << "Upload: \"" << command[1] << "\"\n";

		if (type == 'A') // ascii
		{
			put1FileASCII(command[1]);

		}
		else // binary
		{
			put1File(command[1]);
		}

		type = prevType;

		return true;
	}
	else if (command[0] == "mput") 
	{
		// mput <filePath> <filePath> <filePath> <filePath> <filePath>
		// 
		if (isConnected == false)
		{
			std::cout << "Not connected.\n";
			return true;
		}
		unsigned long long n = command.size();
		if (n < 2)
		{
			return false;
		}

		char prevType = type;

		// upload files
		for (size_t i = 1; i < n; ++i) {
			if (promptMode) {
				std::cout << "Upload: \"" << command[i] << "\"? (Press 'y' to upload): ";
				string res;
				std::getline(cin, res);
				if (res != "y" && res != "Y") continue;
			}

			do
			{
				std::cout << "Select transfer mode: [A] ASCII or [I] Binary: ";

				string inputMode;
				std::getline(cin, inputMode);
				if (inputMode == "A" || inputMode == "a" ||
					inputMode == "ascii" || inputMode == "ASCII")
				{
					// run ascii command
					type = 'A';
					sendCommandMessage("TYPE A\r\n");
					cout << getResponseMessage();

					break;
				}
				else if (inputMode == "I" || inputMode == "i" ||
					inputMode == "binary" || inputMode == "BINARY")
				{
					// run binary command
					type = 'I';
					sendCommandMessage("TYPE I\r\n");
					cout << getResponseMessage();
					break;
				}

				std::cout << "Invalid mode!\n";
			} while (true);

			if (type == 'A') // ascii
			{
				put1FileASCII(command[i]);

			}
			else // binary
			{
				put1File(command[i]);
			}

			std::cout << "\n";
		}

		type = prevType;

		return true;
	}

	else if (command[0] == "ascii")
	{
		if (!isConnected) {
			std::cout << "Not connected.\n"; return true;
		}

		if (command.size() != 1)
		{
			return false;
		}

		type = 'A';
		sendCommandMessage("TYPE A\r\n");
		cout << getResponseMessage();
	}
	else if (command[0] == "binary")
	{
		if (!isConnected) {
			std::cout << "Not connected.\n"; return true;
		}

		if (command.size() != 1)
		{
			return false;
		}

		type = 'I';
		sendCommandMessage("TYPE I\r\n");
		cout << getResponseMessage();
	}

	else if (command[0] == "help" || command[0] == "?")
	{
		if (command.size() != 1) {
			return false;
		}

		std::cout << "================== FTP Client Help Menu ==================\n\n";

		std::cout << "1. File and Directory Operations\n";
		std::cout << "----------------------------------------------------------\n";
		std::cout << "  ls                      List files and folders on the FTP server\n";
		std::cout << "  cd      <dir>           Change current directory on the FTP server\n";
		std::cout << "  pwd                     Show current directory on the FTP server\n";
		std::cout << "  mkdir   <dir>           Create a new directory on the FTP server\n";
		std::cout << "  rmdir   <dir>           Remove a directory from the FTP server\n";
		std::cout << "  delete  <file>          Delete a file on the FTP server\n";
		std::cout << "  rename  <old>   <new>   Rename a file on the FTP server\n\n";

		std::cout << "2. Upload and Download\n";
		std::cout << "----------------------------------------------------------\n";
		std::cout << "  get     <file>          Download a file from the FTP server\n";
		std::cout << "  recv    <file>          Download a file from the FTP server (alias for \"get\")\n";
		std::cout << "  mget    <files>         Download multiple files\n";
		std::cout << "  put     <file>          Upload a file\n";
		std::cout << "  mput    <files>         Upload multiple files\n";
		std::cout << "  prompt                  Toggle confirmation for \"mget/mput\" operations\n\n";

		std::cout << "3. Session Management\n";
		std::cout << "----------------------------------------------------------\n";
		std::cout << "  ascii                   Set transfer mode to ASCII (text files)\n";
		std::cout << "  binary                  Set transfer mode to Binary (non-text files)\n";
		std::cout << "  status                  Show current session status\n";
		std::cout << "  passive                 Toggle passive FTP mode on/off\n";
		std::cout << "  open    <host>          Connect to FTP server\n";
		std::cout << "  close                   Disconnect from FTP server\n";
		std::cout << "  quit                    Exit the FTP client\n";
		std::cout << "  bye                     Exit the FTP client (alias for \"quit\")\n\n";

		std::cout << "4. Misc\n";
		std::cout << "----------------------------------------------------------\n";
		std::cout << "  help                    Show this help menu\n";
		std::cout << "  ?                       Show this help menu (alias for \"help\")\n\n";

		std::cout << "NOTE: If an argument includes spaces, wrap it in double quotes, e.g., \"My File.txt\".\n\n";

		std::cout << "==========================================================\n";

		return true;
	}
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
		//std::cout << responseMessage;
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
		//std::cout << responseMessage;
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


//ham de tao ket data connection cho pasv hoac port
SOCKET SocketClient::establishDataConnection(string& localIP, int& localPort) {
	//passive mode
	if (passiveMode) {
		sendCommandMessage("PASV\r\n");
		string response = getResponseMessage();
		std::cout << response;

		string dataIP, dataPort;
		try {
			tie(dataIP, dataPort) = parsePASVResponse(response);
			return createDataConnection(dataIP, dataPort);
		} 
		catch (const exception& ex) {
			cerr << "PASV error: " << ex.what() << "\n";
			return INVALID_SOCKET;
		}
	}

	//port mode
	else {
		SOCKET listenSocket = createListeningSocket(localIP, localPort);
		if (listenSocket == INVALID_SOCKET) {
			return INVALID_SOCKET;
		}

		string portCommand = formatPORTCommand(localIP, localPort);
		sendCommandMessage(portCommand.c_str());
		std::cout << getResponseMessage();

		return listenSocket;
	}
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

// Tải 1 file từ server về client
void SocketClient::get1File(string filename) {
	std::cout << "Downloading (Binary): " << filename << "\n";

	string localIP;
	int localPort;
	SOCKET dataOrListen = establishDataConnection(localIP, localPort);
	if (dataOrListen == INVALID_SOCKET) {
		cerr << "Failed to create data connection\n";
		return;
	}

	// Gửi lệnh yêu cầu tải file
	sendCommandMessage(("RETR " + filename + "\r\n").c_str());
	std::cout << getResponseMessage(); // 150

	//neu la active thi can accept
	SOCKET dataSocket = passiveMode ? dataOrListen : accept(dataOrListen, nullptr, nullptr);
	if (!passiveMode) {
		closesocket(dataOrListen); //tat cai listen neu la active
	}

	if (dataSocket == INVALID_SOCKET) {
		cerr << "Failed to accept data connection\n";
		return;
	}

	// Kiểm tra thư mục data có tồn tại hay không
	// Cần thư mục data để chứa file tải về (tránh tải trùng tên file)
		//https://www.geeksforgeeks.org/cpp/how-to-check-a-file-or-directory-exists-in-cpp/
	const char* dir = "clientdata";

	struct stat sb;

	// Calls the function with path as argument
	// If the file/directory exists at the path returns 0
	// Nếu thư mục tmp không tồn tại thì tạo thư mục tmp
	if (stat(dir, &sb) != 0)
	{
		string mkdirCommand = "cmd /C \"mkdir " + string(dir) + "\"";

		system(mkdirCommand.c_str());
	}

	// Đổi tên file sang clientdata\filename
	filename = string(dir) + "\\" + filename;



	// tạo file để ghi, trunc để khi mở file sẽ xóa hết nôi dung
	ofstream fout(filename, ios::binary | ios::trunc);
	if (!fout.is_open()) {
		cerr << "Cannot open file to write\n";
		closesocket(dataSocket);
		return;
	}

	// Nhận từng phần dữ liệu và ghi vào file
	char buffer[CHUNK_SIZE];
	int bytesReceived;
	while ((bytesReceived = recv(dataSocket, buffer, sizeof(buffer), 0)) > 0) {
		fout.write(buffer, bytesReceived);
	}

	// Đóng file và socket
	fout.close();
	shutdown(dataSocket, SD_BOTH);
	closesocket(dataSocket);

	// Nhận thông báo cuối cùng
	std::cout << getResponseMessage(); // 226
}

// tai 1 file nhung la che do ascii
void SocketClient::get1FileASCII(string filename) {
	cout << "Downloading (ASCII): " << filename << "\n";

	// lang nghe
	string localIP;
	int localPort;
	SOCKET dataOrListen = establishDataConnection(localIP, localPort);
	if (dataOrListen == INVALID_SOCKET) {
		cerr << "Failed to create data connection\n";
		return;
	}

	// Gửi lệnh yêu cầu tải file
	sendCommandMessage(("RETR " + filename + "\r\n").c_str());
	std::cout << getResponseMessage(); // 150

	//neu la active thi can accept
	SOCKET dataSocket = passiveMode ? dataOrListen : accept(dataOrListen, nullptr, nullptr);
	if (!passiveMode) {
		closesocket(dataOrListen); //tat cai listen neu la active
	}

	if (dataSocket == INVALID_SOCKET) {
		cerr << "Failed to accept data connection\n";
		return;
	}

	// Kiểm tra thư mục data có tồn tại hay không
	// Cần thư mục data để chứa file tải về (tránh tải trùng tên file)
		//https://www.geeksforgeeks.org/cpp/how-to-check-a-file-or-directory-exists-in-cpp/
	const char* dir = "clientdata";

	struct stat sb;

	// Calls the function with path as argument
	// If the file/directory exists at the path returns 0
	// Nếu thư mục tmp không tồn tại thì tạo thư mục tmp
	if (stat(dir, &sb) != 0)
	{
		string mkdirCommand = "cmd /C \"mkdir " + string(dir) + "\"";

		system(mkdirCommand.c_str());
	}

	// Đổi tên file sang clientdata\filename
	filename = string(dir) + "\\" + filename;



	ofstream fout(filename, ios::trunc); // mở file ở chế độ text
	if (!fout.is_open()) {
		cerr << "Cannot open file to write\n";
		closesocket(dataSocket);
		return;
	}

	char buffer[CHUNK_SIZE + 1];	// dam bao ket thuc chuoi
	int bytesReceived;
	while ((bytesReceived = recv(dataSocket, buffer, CHUNK_SIZE, 0)) > 0) {
		buffer[bytesReceived] = '\0';
		fout << buffer;
	}

	fout.close();
	shutdown(dataSocket, SD_BOTH);
	closesocket(dataSocket);

	cout << getResponseMessage(); // 226 Transfer complete (chac vay)
}


void SocketClient::put1File(const string& filePath) // "D:\Folder A\fileA.txt"
{
	//https://cplusplus.com/doc/tutorial/files/
	//  MỞ file
	ifstream fin;
	fin.open(filePath, ios::binary | ios::ate);	//ios::binary   file nhị phân để đọc nội dung -> chuyển nội dung đi
	//ios::ate		mở từ cuối file (để lấy kích thước nhanh hơn)
	if (!fin.is_open())
	{
		std::cout << "File not found or cannot open file.\n";
		return;
	}

	//  Lấy kích thươc tệp (byte)
	uint64_t fileSize = fin.tellg();		//trả về ví trí trong file hiện tại (do mở từ cuối nên sẽ trả về số byte ở cuối, tức là kích thước file)

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
		return;
	}


	//  first message "Scanning: " => kết nối được agent
	std::cout << getResponseMessage(clamavSocket);

	// Gửi loại truyền là Ascii hay binary qua clamav
	sendCommandMessage(clamavSocket, "I");


	//https://youtu.be/NHrk33uCzL8?si=F2rQr1mvSjYWPuHQ
	// 
	//  Chuyển tên file
	sendCommandMessage(clamavSocket, fileName.c_str());
	std::cout << getResponseMessage(clamavSocket);

	//  Chuyển kích thước 
	sendCommandMessage(clamavSocket, to_string(fileSize).c_str());
	std::cout << getResponseMessage(clamavSocket);

	//  Đọc và Chuyển nội dung file theo từng chunk, tránh chuyển đi quá nhiều trong 1 lần
	char buffer[CHUNK_SIZE];
	while (fin)
	{
		// Đọc file binary theo từng chunk 
		fin.read(buffer, CHUNK_SIZE);

		int bytesRead = fin.gcount();		// số Bytes đọc được

		// gửi chunk vừa đọc qua agent
		send(clamavSocket, buffer, bytesRead, 0);
	}

	// Nghỉ chuyển qua agent thì đóng lại
	shutdown(clamavSocket, SD_SEND);


	//  Bên agent: tạo file mới, đọc lại nội dung rồi bỏ vào file
	//Sau đó chạy clamscan file vừa mới tạo. Xong thì xóa file cho đỡ tốn bộ nhớ.


	// in cái dòng "Clamscan result:"
	std::cout << getResponseMessage(clamavSocket);

	//	Lấy kết quả quét
	int iResult = stoi(getResponseMessage(clamavSocket));

	// OK -> chuyển qua server
	if (iResult == 0)
	{
		std::cout << "OK\n";

		// send file to ftp server

		// FTP> put "D:\a a.txt"
		// 
		// command: PORT ...
		// 200 PORT command successful.
		// 
		// command: STOR <file>
		// 150 Starting data transfer.
		// <Gửi nội dung file qua>
		// 226 Operation successful
		// ftp : 5 bytes sent in 0.00Seconds 5.00Kbytes / sec.


		// Lấy ip, port server (cái mà client đang kết nối vô)
		// Phần này giống trong command "ls"
		string localIP;
		int localPort;
		SOCKET dataOrListen = establishDataConnection(localIP, localPort);
		if (dataOrListen == INVALID_SOCKET) {
			cerr << "Failed to create data connection\n";
			return;
		}

		// Send STOR command:
		sendCommandMessage(("STOR " + fileName + "\r\n").c_str());
		cout << getResponseMessage();  // 150

		SOCKET dataSocket = passiveMode ? dataOrListen : accept(dataOrListen, nullptr, nullptr);
		if (!passiveMode) {
			closesocket(dataOrListen);
		}
		if (dataSocket == INVALID_SOCKET) {
			cerr << "Failed to accept data connection\n";
			return;
		}

		//https://stackoverflow.com/questions/5343173/returning-to-beginning-of-file-after-getline
		//  Dời tới đầu file để đọc
		fin.clear();		// do lúc gửi cho agent đã chạm tới eof -> clear
		fin.seekg(0, ios::beg);


		// Đọc và chuyển nội dung qua server
		while (!fin.eof())
		{
			// Đọc file binary theo từng chunk 
			fin.read(buffer, CHUNK_SIZE);

			int bytesRead = fin.gcount();		// số Bytes đọc được

			// gửi chunk vừa đọc qua server
			send(dataSocket, buffer, bytesRead, 0);
		}

		//dong cai data socket
		shutdown(dataSocket, SD_BOTH);
		closesocket(dataSocket);

		// 226 Operation successful
		std::cout << getResponseMessage();
	}
	// Nếu có virus (1) hoặc lỗi (2) thì không gửi qua server
	else if (iResult == 1)
	{
		std::cout << "WARNING: Clamscan found threats!\n";
	}
	else
	{
		std::cout << "ERROR: Scan failed!\n";
		return;
	}


	fin.close();
	closesocket(clamavSocket);
}

void SocketClient::put1FileASCII(const string& filePath) // "D:\Folder A\fileA.txt"
{
	//https://cplusplus.com/doc/tutorial/files/
	//  MỞ file
	ifstream fin;
	fin.open(filePath);
	if (!fin.is_open())
	{
		std::cout << "File not found or cannot open file.\n";
		return;
	}

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
		return;
	}


	//  first message "Scanning: " => kết nối được agent
	std::cout << getResponseMessage(clamavSocket);


	// Gửi loại truyền là Ascii hay binary qua clamav
	sendCommandMessage(clamavSocket, "A");


	//https://youtu.be/NHrk33uCzL8?si=F2rQr1mvSjYWPuHQ
	// 
	//  Chuyển tên file
	sendCommandMessage(clamavSocket, fileName.c_str());
	std::cout << getResponseMessage(clamavSocket) << "\n";

	// ASCII nên chuyển theo dòng
	string buffer;
	while (std::getline(fin, buffer))
	{
		//thêm endline để gửi nội dung có sẵn ngăn dòng (nếu không thì thông tin bị dính liền nhau)
		buffer += "\n";		

		send(clamavSocket, buffer.c_str(), buffer.size(), 0);
	}

	// Nghỉ chuyển thì đóng (tránh việc bên kia đợi recv)
	shutdown(clamavSocket, SD_SEND);


	//  Bên agent: tạo file mới, đọc lại nội dung rồi bỏ vào file
	//Sau đó chạy clamscan file vừa mới tạo. Xong thì xóa file cho đỡ tốn bộ nhớ.


	// in cái dòng "Clamscan result:"
	std::cout << getResponseMessage(clamavSocket);

	//	Lấy kết quả quét
	int iResult = stoi(getResponseMessage(clamavSocket));

	// OK -> chuyển qua server
	if (iResult == 0)
	{
		std::cout << "OK\n";

		// send file to ftp server

		// FTP> put "D:\a a.txt"
		// 
		// command: PORT ...
		// 200 PORT command successful.
		// 
		// command: STOR <file>
		// 150 Starting data transfer.
		// <Gửi nội dung file qua>
		// 226 Operation successful
		// ftp : 5 bytes sent in 0.00Seconds 5.00Kbytes / sec.


		// Lấy ip, port server (cái mà client đang kết nối vô)
		// Phần này giống trong command "ls"
		string localIP;
		int localPort;
		SOCKET dataOrListen = establishDataConnection(localIP, localPort);
		if (dataOrListen == INVALID_SOCKET) {
			cerr << "Failed to create data connection\n";
			return;
		}

		// Send STOR command:
		sendCommandMessage(("STOR " + fileName + "\r\n").c_str());
		cout << getResponseMessage();  // 150

		SOCKET dataSocket = passiveMode ? dataOrListen : accept(dataOrListen, nullptr, nullptr);
		if (!passiveMode) {
			closesocket(dataOrListen);
		}
		if (dataSocket == INVALID_SOCKET) {
			cerr << "Failed to accept data connection\n";
			return;
		}

		//https://stackoverflow.com/questions/5343173/returning-to-beginning-of-file-after-getline
		//  Dời tới đầu file để đọc
		fin.clear();		// do lúc gửi cho agent đã chạm tới eof -> clear
		fin.seekg(0, ios::beg);


		// Đọc và chuyển nội dung qua server
		while (std::getline(fin, buffer))
		{
			buffer += "\n";

			send(dataSocket, buffer.c_str(), buffer.size(), 0);
		}
		//dong cai data socket
		shutdown(dataSocket, SD_BOTH);	// không chuyển nội dung file nữa nên đóng (đóng SD_SEND cũng được)
		closesocket(dataSocket);

		// 226 Operation successful
		std::cout << getResponseMessage();
	}
	// Nếu có virus (1) hoặc lỗi (2) thì không gửi qua server
	else if (iResult == 1)
	{
		std::cout << "WARNING: Clamscan found threats!\n";
	}
	else
	{
		std::cout << "ERROR: Scan failed!\n";
		return;
	}


	fin.close();
	closesocket(clamavSocket);
}
