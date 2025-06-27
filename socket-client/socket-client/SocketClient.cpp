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


bool SocketClient::connectToServer(addrinfo* result) {
	int iResult{};		// for checking
	// attempt to connect to the server
	for (int num_attempt = 0; num_attempt < 10; ++num_attempt) {
		this->socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (this->socket_ == INVALID_SOCKET) { // fail to create socket
			std::cerr << "Socket failed: " << WSAGetLastError() << "\n";
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// connect to the server
		iResult = connect(this->socket_, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == 0) { // success
			break;
		}

		// fail to connect to the server
		std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
		closesocket(this->socket_);
		this->socket_ = INVALID_SOCKET;
	}

	if (socket_ == INVALID_SOCKET) { // fail to connect
		std::cerr << "Unable to connect to server!" << std::endl;
		return false;
	}

	return true;
}

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


		//hints: give the supported type (IPV4 TCP)
		addrinfo* result = NULL, hints;
		memset(&hints, 0, sizeof(hints));	//clear all data of hints
		//IPV4 and TCP
		hints.ai_family = AF_INET;			//ipv4
		hints.ai_socktype = SOCK_STREAM;	//tcp protocol
		hints.ai_protocol = IPPROTO_TCP;	//tcp protocol

		// get the server address and port, put to result
		int iResult = getaddrinfo(serverIP.c_str(), port, &hints, &result);
		if (iResult != 0) { //fail to get address
			std::cerr << "getaddrinfo failed: " << iResult << std::endl;
			isQuit = true;
			return true;
		}

		// attemp to connect
		if (connectToServer(result) == false) {
			freeaddrinfo(result);
			isQuit = true;
			return true;
		}

		// connected
		isConnected = true;
		std::cout << "Connected to " << serverIP << endl;
		freeaddrinfo(result);

		getResponseMessage();

		// input username and password
		sendCommandMessage("OPTS UTF8 ON\r\n");
		getResponseMessage();

		cout << "Username: ";
		getline(cin, username);

		string tempMsg = "USER " + username + "\r\n";

		sendCommandMessage(tempMsg.c_str());
		getResponseMessage();

		cout << "Password: ";
		getline(cin, password);
		tempMsg = "PASS " + password + "\r\n";

		sendCommandMessage(tempMsg.c_str());
		getResponseMessage();

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
		getResponseMessage();

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
		getResponseMessage();

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
		getResponseMessage();

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
		getResponseMessage();

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
		getResponseMessage();

		return true;
	}
	
	//else if (command[0] == "ls") {}
	//else if (command[0] == "rmdir") {}
	//else if (command[0] == "delete") {}
	//else if (command[0] == "rename") {}
	//else if (command[0] == "get" || command[0] == "recv") {}
	//else if (command[0] == "put") {}
	//else if (command[0] == "mput") {}
	//else if (command[0] == "mget") {}
	//else if (command[0] == "prompt") {}
	//else if (command[0] == "ascii / binary") {}
	//else if (command[0] == "status") {}
	//else if (command[0] == "passive") {}
	//else if (command[0] == "help" || command[0] == "?") {}

	
	else
	{
		return false;
	}
}

void SocketClient::getResponseMessage()
{
	// get response message
	char responseMessage[4097];		// leave 1 char for '\0'

	int iResult = recv(socket_, responseMessage, sizeof(responseMessage) - 1, 0);
	
	if (iResult > 0) {
		responseMessage[iResult] = '\0';
	}

	cout << responseMessage;
}

void SocketClient::sendCommandMessage(const char* msg)
{
	int iResult = send(socket_, msg, (int)strlen(msg), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		isQuit = true;
	}
}

