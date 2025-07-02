#include "General.h"

vector<string> processCommandString(string command)
{
	vector<string> res;

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

	return res;
}