#include "General.h"

vector<string> processCommandString(string command)
{
	vector<string> res;
	istringstream iss(command);

	string word;
	while (iss >> word)
	{
		res.push_back(word);
	}

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
