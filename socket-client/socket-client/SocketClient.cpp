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

// invalid command => return false
bool SocketClient::processCommand()
{
	//connect to the FTP server
	if (command[0] == "open") // open <IP>
	{
		if (isConnected == true)
		{
			cout << "Already connected to " << serverIP << ", disconnect first.\n";
			return true;
		}

		if (command.size() != 2)
		{
			//cout << "Invalid command.\n";
			return false;
		}

		serverIP = command[1];
		if (isValidIP(serverIP) == false)
		{
			return true;
		}


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

		SOCKET newSocket = createConnection(serverIP, port, true);  // true for retry
		if (newSocket == INVALID_SOCKET) {
			isQuit = true;
			return true;
		}

		socket_ = newSocket;
		isConnected = true;
		cout << "Connected to " << serverIP << endl;

		cout << getResponseMessage();

		// input username and password
		sendCommandMessage("OPTS UTF8 ON\r\n");
		cout << getResponseMessage();

		cout << "Username: ";
		getline(cin, username);

		string tempMsg = "USER " + username + "\r\n";

		sendCommandMessage(tempMsg.c_str());
		cout << getResponseMessage();

		cout << "Password: ";
		getline(cin, password);
		tempMsg = "PASS " + password + "\r\n";

		sendCommandMessage(tempMsg.c_str());
		cout << getResponseMessage();

		return true;
	}
	//disconnect to the FTP server
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
	else if (command[0] == "pwd")	//pwd
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

		string folderName = command[1];

		string msg = "CWD " + folderName + "\r\n";

		sendCommandMessage(msg.c_str());
		cout << getResponseMessage();

		return true;
	}
	//Create folder
	else if (command[0] == "mkdir") 
	{
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

		string folderName = command[1];

		string msg = "XMKD " + folderName + "\r\n";

		sendCommandMessage(msg.c_str());
		cout << getResponseMessage();

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

		//1. gui PASV de vao passive mode
		sendCommandMessage("PASV\r\n");
		string response = getResponseMessage();
		cout << response;
		if (response.substr(0, 3) != "227") {
			cerr << "PASV command failed: " << response;
			return true;
		}
		string dataIP, dataPort;

		try {
			tie(dataIP, dataPort) = parsePASVResponse(response);
		}
		catch (const exception& ex) {
			cerr << ex.what() << "\n";
			return false;
		}

		SOCKET dataSocket = createDataConnection(dataIP, dataPort);
		if (dataSocket == INVALID_SOCKET) {
			cerr << "Failed to create data connection \n";
			return true;
		}
		//gui cai lenh NLST
		sendCommandMessage("NLST\r\n");

		//bat dau doc directory tu data socket
		char buffer[4096];
		string directoryListing;
		int bytesReceived;
		while ((bytesReceived = recv(dataSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
			buffer[bytesReceived] = '\0';
			directoryListing += buffer;
		}

		//dong cai data socket
		closesocket(dataSocket);
		//list cai directory ra
		if (!directoryListing.empty()) {
			cout << directoryListing << "\n";
		}

		string finalMessage = getResponseMessage();
		cout << finalMessage;

		return true;
	}
	//else if (command[0] == "rmdir") {}
	//else if (command[0] == "delete") {}
	//else if (command[0] == "rename") {}
	//else if (command[0] == "get" || command[0] == "recv") {}
	//else if (command[0] == "mget") {}
	//else if (command[0] == "prompt") {}
	//else if (command[0] == "ascii / binary") {}
	//else if (command[0] == "status") {}
	//else if (command[0] == "passive") {}
	//else if (command[0] == "help" || command[0] == "?") {}

	

	// clamscan agent.....................

	//else if (command[0] == "put") {}
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

void SocketClient::sendCommandMessage(const char* msg)
{
	int iResult = send(socket_, msg, (int)strlen(msg), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		isQuit = true;
	}
}

SOCKET SocketClient::createConnection(const string& ip, const string& port, bool withRetry) {
	SOCKET newSocket = INVALID_SOCKET;
	addrinfo* result = NULL;
	addrinfo hints;

	//init hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;        //ipv4
	hints.ai_socktype = SOCK_STREAM;  //tcp
	hints.ai_protocol = IPPROTO_TCP;  //tcp

	//lay dia chi
	int iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		cerr << "getaddrinfo failed: " << iResult << endl;
		return INVALID_SOCKET;
	}

	//so lan thu, do t thay m de 10 lan, muon tat thi tat cai withRetry
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