#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <regex>
#include <tuple>

using namespace std;

vector<string> processCommandString(string command);

bool isValidIP(string ip);

bool isNum(string temp);

pair<string, string> parsePASVResponse(const string& response);
