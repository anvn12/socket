#include "General.h"

vector<string> processCommandString(string command)
{
	vector<string> res;
	//istringstream iss(command);

	string word = "";

	for (int i = 0; i < command.size(); i++)
	{
		if (command[i] == '"')
		{
			++i;
			while (i < command.size() && command[i] != '"')
			{
				word.push_back(command[i]);
				++i;
			}
		}
		else if (command[i] == ' ' && word != "")
		{
			res.push_back(word);
			word = "";
		}
		else
		{
			word.push_back(command[i]);
		}

	}
	if (word != "")
	{
		res.push_back(word);
	}

	//while (iss >> word)
	//{
		//res.push_back(word);
	//}

	return res;
}

bool isValidIP(string ip)
{
	//vector<string> container;

	int count = 0;

	istringstream iss(ip);
	string temp;
	while (getline(iss, temp, '.'))
	{
		if (isNum(temp) == false)
		{
			cerr << "Invalid IP\n";
			return false;
		}

		int num = stoi(temp);

		if (num < 0 || num > 255)
		{
			cerr << "Invalid IP\n";
			return false;
		}

		count++;
		//container.push_back(temp);
	}

	if (count != 4)
	{
		cerr << "Invalid IP\n";
		return false;
	}

	return true;
}

bool isNum(string temp)
{
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (!isdigit(temp[i]))
		{
			return false;
		}
	}

	return true;
}

pair<string, string> parsePASVResponse(const string& response) {
	//dung regex de khoi pai parse () hay ,
	//227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
	regex pasvRegex(R"(\((\d+),(\d+),(\d+),(\d+),(\d+),(\d+)\))");
	smatch match;

	if (regex_search(response, match, pasvRegex) && match.size() == 7) {
		int h1 = stoi(match[1]);
		int h2 = stoi(match[2]);
		int h3 = stoi(match[3]);
		int h4 = stoi(match[4]);
		int p1 = stoi(match[5]);
		int p2 = stoi(match[6]);

		string ip = to_string(h1) + "." + to_string(h2) + "." + to_string(h3) + "." + to_string(h4);
		int intPort = p1 * 256 + p2;
		string port = to_string(intPort);
		return { ip, port };
	}
	else {
		throw runtime_error("Failed to parse PASV: " + response);
	}
}


////promtMsg la cai ma muon in ra vd Folder name: dir name:, ...
//string getArgOrPrompt(const vector<string>& command, size_t startIndex, const string& promptMsg) {
//	string name;
//	if (command.size() <= startIndex) {
//		cout << promptMsg;
//		getline(cin, name);
//		if (name.empty()) {
//			return "";
//		}
//	}
//	//ghep tu cai startIndex vao
//	else {
//		name = command[startIndex];
//		for (size_t i = startIndex + 1; i < command.size(); ++i) {
//			name += " " + command[i];
//		}
//	}
//	//xoa may cai dau ""
//	if (name.size() > 1 && name.front() == '"' && name.back() == '"') {
//		name = name.substr(1, name.size() - 2);
//	}
//	return name;
//}
